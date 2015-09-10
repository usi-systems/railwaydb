#include <intergdb/simulation/ExpSetupHelper.h>

#include <intergdb/core/InteractionGraph.h>
#include <intergdb/common/Types.h>

#include <intergdb/util/ZipfRand.h>
#include <intergdb/util/NormalRand.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <algorithm>
#include <unordered_set>
#include <queue>

#include <intergdb/util/AutoTimer.h>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::simulation;

namespace intergdb { namespace simulation {

random_device ExpSetupHelper::randomDevice_;
mt19937 ExpSetupHelper::randomGen_(randomDevice_());
uniform_int_distribution<int> ExpSetupHelper::seedDist_(
    1, numeric_limits<int>::max());

ostream& operator<<(ostream& ostr, Tweet const& tweet)
{
    ostr << "<";
    ostr << "time: " << tweet.time << ", ";
    ostr << "tweetId: " << tweet.tweetId << ", ";
    ostr << "userId: " << tweet.userId << ", ";
    ostr << "retweetId: " << tweet.retweetId << ", ";
    ostr << "inReplyToStatusId: " << tweet.inReplyToStatusId << ", ";
    ostr << "isTruncated: " << tweet.isTruncated << ", ";
    ostr << "mentionedUsers: " << tweet.mentionedUsers << ", ";
    ostr << "hashTags: " << tweet.hashTags << ", ";
    ostr << "text: " << tweet.text;
    ostr << ">";
    return ostr;
}
} }

void ExpSetupHelper::purge() {
#ifdef __MACH__
    system("sudo purge");
#else
    cerr << "purge not supported on linux" << endl;
#endif
}

Conf ExpSetupHelper::createGraphConf(string const& dbDirPath,
                                     string const& expName)
{
    boost::filesystem::path expDir = dbDirPath;
    expDir = expDir / expName;
    Conf conf("exp", expDir.string(),
            {{"v", DataType::INT64}},
            {
                {"dir", DataType::STRING},
                {"time", DataType::STRING},
                {"tweetId", DataType::STRING},
                {"userId", DataType::STRING},
                {"retweetId", DataType::STRING},
                {"inReplyToStatusId", DataType::STRING},
                {"isTruncated", DataType::STRING},
                {"mentionedUsers", DataType::STRING},
                {"hashTags", DataType::STRING},
                {"text", DataType::STRING}
            });
    return conf;
}

static bool parseTweet(string const & line,
    int64_t & from, vector<int64_t> & tos, Tweet & tweet)
{
    using namespace boost;
    size_t s = 0;
    size_t e = line.find('|');
    if (e==string::npos)
        return false;
    tweet.time = line.substr(0, e);
    e = line.find('|', s = e + 1);
    tweet.tweetId = line.substr(s, e - s);
    e = line.find('|', s = e + 1);
    tweet.userId = line.substr(s, e - s);
    from = lexical_cast<int64_t>(tweet.userId);
    e = line.find('|', s = e + 1);
    tweet.retweetId = line.substr(s, e - s);
    e = line.find('|', s = e + 1);
    tweet.inReplyToStatusId = line.substr(s, e - s);
    e = line.find('|', s = e + 1);
    tweet.isTruncated = line.substr(s, e - s);
    e = line.find('|', s = e + 1);
    tweet.mentionedUsers = line.substr(s, e - s);
    e = line.find('|', s = e + 1);
    tweet.hashTags = line.substr(s, e - s);
    tweet.text = line.substr(s = e + 1);
    //
    string token;
    tos.push_back(-from);
    for (s = 0; e != string::npos; s = e + 1) {
        e = tweet.mentionedUsers.find(',', s);
        if (e == string::npos)
            token = tweet.mentionedUsers.substr(s);
        else
            token = tweet.mentionedUsers.substr(s, e-s);
        if (!token.empty()) {
            int64_t to = lexical_cast<int64_t>(token);
            tos.push_back(to);
        }
    }
    return true;
}

static uint64_t getDayStartTime(string const& dt)
{
    // 20130514
    using namespace boost;
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    int year = lexical_cast<int>(dt.substr(0,4));
    int month = lexical_cast<int>(dt.substr(4,2));
    int day = lexical_cast<int>(dt.substr(6,2));
    ptime tm(date(year, month, day), hours(0));
    ptime epoch(date(1970,1,1));
    return (tm - epoch).total_seconds();
}

static uint64_t getTweetTimestamp(string const& td, uint64_t dayStart)
{
    // 16:43:47
    using namespace boost;
    using namespace boost::posix_time;
    int hour = lexical_cast<int>(td.substr(0,2));
    int min = lexical_cast<int>(td.substr(3,2));
    int sec = lexical_cast<int>(td.substr(6,2));
    time_duration d = hours(hour) + minutes(min) + seconds(sec);
    return (dayStart+d.total_seconds()) * 1000.0;
}


void ExpSetupHelper::scanTweets(string const & dirPath,
    function<void (uint64_t time,
                   int64_t from,
                   vector<int64_t> const& tos,
                   Tweet const& tweets)> visitor,
                   uint64_t& tsStart,
                   uint64_t& tsEnd)
{
    using namespace boost::filesystem;
    tsStart = numeric_limits<uint64_t>::max();
    tsEnd = numeric_limits<uint64_t>::min();

    path tweetDir(dirPath);
    vector<string> fileNames;
    for (directory_iterator it(tweetDir), eit = directory_iterator();
            it != eit; ++it) {
        path tweetFile = it->path();
        fileNames.push_back(tweetFile.string());
    }
    sort(fileNames.begin(), fileNames.end());
    size_t repeat = 0;
    int64_t pts = -1;
    Tweet tweet;
    for (auto const & fileName : fileNames) {
        ifstream file(fileName.c_str());
        string date = path(fileName).filename().string();
        date = date.substr(date.find('_')+1, 8);
        uint64_t dayTime = getDayStartTime(date);
        string line;
        do {
            std::getline(file, line);
            string time;
            int64_t from;
            vector<int64_t> tos;

            if (!parseTweet(line, from, tos, tweet))
                continue;
            uint64_t ts = getTweetTimestamp(tweet.time, dayTime);
            if (ts!=pts) {
                repeat = 0;
                pts = ts;
            }
            if (ts <= tsStart) tsStart = ts;
            if (ts >= tsEnd) tsEnd = ts;
            visitor(ts+repeat, from, tos, tweet);
            repeat += tos.size();
        } while(!file.eof());
    }

}

void ExpSetupHelper::populateGraphFromTweets(string const& dirPath,
    vector<unique_ptr<core::InteractionGraph>> & graphs,
    uint64_t& tsStart,
    uint64_t& tsEnd,
    unordered_set<int64_t> & vertices)
{
    using namespace boost;
    {
        scanTweets(dirPath, [&] (uint64_t time,
            int64_t from, vector<int64_t> const& tos, Tweet const& tweet)
        {
            vertices.insert(from);
            for (int64_t to : tos)
                vertices.insert(to);
        }, tsStart, tsEnd);
        for (int64_t v : vertices) {
            VertexId vi = v;
            for (auto iter = graphs.begin(); iter != graphs.end(); ++iter) {
                (*iter)->createVertex(vi, v);
            }
        }
    }
    {
        int i = 0;
        Timestamp lastTimestamp = 0;
        scanTweets(dirPath, [&] (uint64_t timestamp,
            int64_t from, vector<int64_t> const& tos, Tweet const& tweet)
        {
            if (timestamp != lastTimestamp)
            {
                i = 0;
                lastTimestamp = timestamp;
            }
            for (int64_t to : tos) {
                string dir = (from>to) ? "l" : "s";
                if (from != to) {
                    for (auto iter = graphs.begin(); iter != graphs.end();
                         ++iter) {
                        (*iter)->addEdge(from, to, timestamp + i,
                            dir, tweet.time, tweet.tweetId, tweet.userId,
                            tweet.retweetId, tweet.inReplyToStatusId,
                            tweet.isTruncated, tweet.mentionedUsers,
                            tweet.hashTags, tweet.text);
                    }
                    i++;
                }
            }
        }, tsStart, tsEnd);
    }
    for (auto iter = graphs.begin(); iter != graphs.end(); ++iter) {
        (*iter)->flush();
    }
}

vector<FocusedIntervalQuery> ExpSetupHelper::genSearchQueries(
    vector<vector<string>> const & templates,
    double queryZipfParam,
    int numQueries,
    uint64_t tsStart,
    uint64_t tsEnd,
    double delta,
    InteractionGraph * graph,
    bool const useMostProlificVertex/*=false*/)
{
    vector<core::FocusedIntervalQuery> queries;

    int numQueryTypes = templates.size();
    util::ZipfRand queryGen(queryZipfParam, numQueryTypes);
    queryGen.setSeed(seedDist_(randomGen_));

    uint64_t startTime, endTime;
    uint64_t const duration = static_cast<uint64_t>(
        delta * static_cast<double>(tsEnd - tsStart));
    // use a random start time for the interval query
    uniform_int_distribution<uint64_t> timeDist(tsStart, tsEnd - duration);

    vector<VertexId> vertices;
    while (vertices.empty())
    {
        startTime = timeDist(randomGen_);
        endTime = startTime + duration;
        // find out the vertices that have interactions in the time range
        graph->processIntervalQueryBatch(
            IntervalQuery(startTime, endTime), vertices);
    }
    vector<VertexId> vertexList;
    for (auto const& vertex : vertices) {
        if (static_cast<make_signed<VertexId>::type>(vertex) < 0)
            continue;
        vertexList.push_back(vertex);
    }

    VertexId mostProlificVertex;
    size_t maxOccurances = 0;
    if (useMostProlificVertex) {
        for (auto const& vertex : vertexList) {
            FocusedIntervalQuery q(vertex, startTime, endTime, templates[0]);
            size_t count = 0;
            for (auto iqIt = graph->processFocusedIntervalQuery(q);
                 iqIt.isValid(); iqIt.next()) {
                count += 1;
            }
            if (count > maxOccurances) {
                maxOccurances = count;
                mostProlificVertex = vertex;
            }
        }
        cerr << "XXX " << maxOccurances << endl;
    }

    // use a random start node for the interval query
    uniform_int_distribution<int> vertexDist(0, vertexList.size() - 1);

    for (int i = 0; i < numQueries; i++) {
        int templateIndex = numQueryTypes > 1 ? queryGen.getRandomValue() : 0;
        int vertexIndex = vertexDist(randomGen_);
        VertexId vi = useMostProlificVertex ?  mostProlificVertex :
            vertexList.at(vertexIndex);
        queries.push_back(FocusedIntervalQuery(
            vi, startTime, endTime, templates[templateIndex]));
    }
    return queries;
}

double ExpSetupHelper::runWorkload(
    InteractionGraph * graph,
    vector<core::FocusedIntervalQuery> const & queries)
{
    int count = 0;
    int sizes = 0;
    util::AutoTimer timer;
    double duration = 0.0;

    for (auto q : queries) {
        graph->clearBlockBuffer();
        timer.start();
        for (auto iqIt = graph->processFocusedIntervalQuery(q);
             iqIt.isValid(); iqIt.next()) {
            sizes += iqIt.getEdgeData()->getFields().size();
            count += 1;
        }
        timer.stop();
        duration += timer.getRealTimeInSeconds();
    }
    assert (count != 0);
    assert (sizes != 0);
    return duration;
}

double ExpSetupHelper::runDFS(
    InteractionGraph * graph,
    vector<core::FocusedIntervalQuery> const & queries)
{
    util::AutoTimer timer;
    timer.start();
    for (auto q : queries) {
        graph->clearBlockBuffer();
        set<VertexId> visited;
        dfs(graph, q, visited);
    }
    timer.stop();
    return timer.getRealTimeInSeconds();
}

double ExpSetupHelper::runBFS(
    InteractionGraph * graph,
    vector<core::FocusedIntervalQuery> const & queries)
{
    util::AutoTimer timer;
    timer.start();
    for (auto q : queries) {
        graph->clearBlockBuffer();
        bfs(graph, q);
    }
    timer.stop();
    return timer.getRealTimeInSeconds();
}

void ExpSetupHelper::dfs(
    InteractionGraph * graph,
    FocusedIntervalQuery const & query,
    set<VertexId> & visited)
{
    int count = 0;
    int sizes = 0;
    visited.insert(query.getHeadVertex());
    for (auto iqIt = graph->processFocusedIntervalQuery(query);
         iqIt.isValid(); iqIt.next()) {
        VertexId u = iqIt.getToVertex();
        sizes += iqIt.getEdgeData()->getFields().size();
        count++;
        auto search = visited.find(u);
        if(search != visited.end()) {
            FocusedIntervalQuery next(u, query.getStartTime(),
                query.getEndTime(), query.getAttributeNames());
            dfs(graph, next, visited);
        }
    }
}

void ExpSetupHelper::bfs(
    InteractionGraph * graph,
    FocusedIntervalQuery const & query )
{
    int count = 0;
    int sizes = 0;
    queue<VertexId> q;
    set<VertexId> visited ;
    VertexId u,t;

    q.push(query.getHeadVertex());
    visited.insert(query.getHeadVertex());

    while (!q.empty()) {
        u = q.front();
        q.pop();
        FocusedIntervalQuery next(u, query.getStartTime(), query.getEndTime(), query.getAttributeNames());
	for (auto iqIt = graph->processFocusedIntervalQuery(query); iqIt.isValid(); iqIt.next()) {
            sizes += iqIt.getEdgeData()->getFields().size();
            count += 1;
            t = iqIt.getToVertex();
            auto search = visited.find(t);
            if(search != visited.end()) {
                q.push(t);
                visited.insert(t);
            }
        }
    }
}

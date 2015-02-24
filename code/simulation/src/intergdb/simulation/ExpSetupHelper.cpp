#include <intergdb/simulation/ExpSetupHelper.h>

#include <intergdb/core/InteractionGraph.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <algorithm>
#include <unordered_set>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::simulation;

namespace intergdb { namespace simulation {
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
    conf.blockSize() = 1024; // num bytes
    conf.windowSize() = 10*1000; // num edges
    conf.blockBufferSize() = 100; // num blocks
    conf.vertexDataBufferSize() = 100*1000; // num verices
    conf.expirationMapSize() = 1000; // num edges (0.1)
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
                   int64_t from, vector<int64_t> const& tos,
                   Tweet const& tweets)> visitor)
{
    using namespace boost::filesystem;
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
            visitor(ts+repeat, from, tos, tweet);
            repeat += tos.size();
        } while(!file.eof());
    }
}

void ExpSetupHelper::populateGraphFromTweets(string const& dirPath,
    InteractionGraph & graph)
{
    using namespace boost;
    {
        std::unordered_set<int64_t> vertices;
        scanTweets(dirPath, [&] (uint64_t time,
            int64_t from, vector<int64_t> const& tos, Tweet const& tweet)
        {
            vertices.insert(from);
            for (int64_t to : tos)
                vertices.insert(to);
        });
        for (int64_t v : vertices) {
            VertexId vi = v;
            graph.createVertex(vi, v);
        }
    }
    {
        scanTweets(dirPath, [&] (uint64_t time,
            int64_t from, vector<int64_t> const& tos, Tweet const& tweet)
        {
            int i = 0;
            for (int64_t to : tos) {
                string dir = (from>to) ? "l" : "s";
                if (from != to) {
                    graph.addEdge(from, to, time+(i++),
                        dir, tweet.time, tweet.tweetId, tweet.userId,
                        tweet.retweetId, tweet.inReplyToStatusId,
                        tweet.isTruncated, tweet.mentionedUsers,
                        tweet.hashTags, tweet.text);
                }
            }
        });
    }
}



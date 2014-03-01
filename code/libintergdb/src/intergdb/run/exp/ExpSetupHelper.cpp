#include <intergdb/run/exp/ExpSetupHelper.h>

#include <intergdb/gen/InteractionGraphGenerator.h>
#include <intergdb/core/Conf.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include "boost/date_time/posix_time/posix_time_types.hpp"

#include <algorithm>
#include <unordered_set>

using namespace std;
using namespace intergdb::gen;
using namespace intergdb::core;
using namespace intergdb::run::exp;

string ExpSetupHelper::getString(size_t size)
{
    static vector<char> data;
    if (data.size()!=size)
        data.resize(size);
    return string(&data[0], size);
}

static boost::filesystem::path getDataDir()
{
    return boost::filesystem::path("data");
}

void ExpSetupHelper::clearDBFilesForTheExperiment(string const & expName)
{
    boost::filesystem::path expDir = getDataDir() / expName;
    boost::filesystem::remove_all(expDir);
    boost::filesystem::create_directory(expDir);
}

void ExpSetupHelper::setupGraphGeneratorWithDefaults(InteractionGraphGenerator::Conf & conf)
{
    conf.popularityGroupCount() = 10000;
    conf.popularityZipfParam() = 1.5;
    conf.graphType() = InteractionGraphGenerator::ScaleFree;
    conf.numVertices() = 100*1000;
    conf.numEdges() = 10*conf.numVertices();
    conf.interArrivalTimeMean() = InteractionGraphGenerator::Conf::
        convertPerDayCountToInterArrivalDelayInMillisecs(100*conf.numVertices());
}

Conf ExpSetupHelper::createGraphConfWithDefaults(string const & expName)
{
    boost::filesystem::path expDir = getDataDir() / expName;
    Conf conf("exp", expDir.string());
    conf.blockSize() = 1024; // num bytes
    conf.windowSize() = 10*1000; // num edges
    conf.blockBufferSize() = 100; // num blocks
    conf.vertexDataBufferSize() = 100*1000; // num verices
    conf.expirationMapSize() = 1000; // num edges (0.1)
    return conf;
}

static bool parseTweet(string const & line, string & time,
        int64_t & from, vector<int64_t> & tos,
        string & id, string & data)
{
    using namespace boost;
    size_t s = 0;
    size_t e = line.find('|');
    if (e==string::npos)
        return false;
    time = line.substr(0, e);
    for (int i=0; i<1; ++i) {
        s = e + 1;
        e = line.find('|', s);
    }
    id = line.substr(s, e-s);
    for (int i=0; i<1; ++i) {
        s = e + 1;
        e = line.find('|', s);
    }
    from = lexical_cast<int64_t>(line.substr(s, e-s));
    for (int i=0; i<4; ++i) {
        s = e + 1;
        e = line.find('|', s);
    }
    string toList = line.substr(s, e-s);
    for (int i=0; i<1; ++i) {
        s = e + 1;
        e = line.find('|', s);
    }
    s = e+1;
    data = line.substr(s);
    //
    string token;
    tos.push_back(-from);
    for (s=0; e!=string::npos; s=e+1) {
        e = toList.find(',', s);
        if (e==string::npos)
            token = toList.substr(s);
        else
            token = toList.substr(s, e-s);
        if (!token.empty()) {
            int64_t to = lexical_cast<int64_t>(token);
            tos.push_back(to);
        }
    }
    return true;
}

static uint64_t getDayStartTime(string const & dt)
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

static uint64_t getTweetTimestamp(string const & td, uint64_t dayStart)
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
        function<void (uint64_t, int64_t, vector<int64_t> const &,
                       string const &, string const &)> visitor)
{
    using namespace boost::filesystem;
    path tweetDir(dirPath);
    vector<string> fileNames;
    for(directory_iterator it(tweetDir), eit = directory_iterator();
            it != eit; ++it) {
        path tweetFile = it->path();
        fileNames.push_back(tweetFile.string());
    }
    sort(fileNames.begin(), fileNames.end());
    size_t repeat = 0;
    int64_t pts = -1;
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
            string id, data;
            if(!parseTweet(line, time, from, tos, id, data))
                continue;
            uint64_t ts = getTweetTimestamp(time, dayTime);
            if (ts!=pts) {
                repeat = 0;
                pts = ts;
            }
            visitor(ts+repeat, from, tos, id, data);
            repeat += tos.size();
        } while(!file.eof());
    }
}

void ExpSetupHelper::populateGraphFromTweets(string const & dirPath,
                    InteractionGraph<int64_t, string> & graph,
                    size_t * nVertices/*=nullptr*/, size_t * nEdges/*=nullptr*/)
{
    using namespace boost;
    /*
    {
        scanTweets(dirPath, [&] (uint64_t time,
                    int64_t from, vector<int64_t> const & tos,
                    string const & id, string const & data)
        {
            cerr << time << ", " << id << ",";
            cerr << from << " -> [";
            for (auto const & to : tos)
                cerr << to << ", ";
            cerr <<  "], ";
            cerr << data << "\n";
        });
    }
    */
    {
        std::unordered_set<int64_t> vertices;
        scanTweets(dirPath, [&] (uint64_t time,
                int64_t from, vector<int64_t> const & tos,
                string const & id, string const & data)
        {
            vertices.insert(from);
            for (int64_t to : tos)
                vertices.insert(to);
        });
        for (int64_t v : vertices) {
            VertexId vi = v;
            graph.createVertex(vi, v);
        }
        if (nVertices!=nullptr)
            *nVertices = vertices.size();
    }
    {
        size_t ne = 0;
        scanTweets(dirPath, [&] (uint64_t time,
                int64_t from, vector<int64_t> const & tos,
                string const & id, string const & data)
        {
            int i = 0;
            for (int64_t to : tos) {
                string dir = (from>to) ? "l" : "s";
                string edata = dir + "_" + id + "_" + data;
                graph.addEdge(from, to, edata, time+i);
                ne++; i++;
            }
        });
        if (nEdges!=nullptr)
            *nEdges = ne;
    }
}



#pragma once

#include <intergdb/core/Conf.h>
#include <intergdb/core/InteractionGraph.h>
#include <intergdb/core/Query.h>

#include <random>
#include <string>
#include <vector>
#include <set>

namespace intergdb { namespace simulation {

    struct Tweet
    {
        std::string time;
        std::string tweetId;
        std::string userId;
        std::string retweetId;
        std::string inReplyToStatusId;
        std::string isTruncated;
        std::string mentionedUsers;
        std::string hashTags;
        std::string text;
    };

    std::ostream& operator<<(std::ostream& ostr, Tweet const& tweet);

    class ExpSetupHelper
    {
    public:
        static core::Conf createGraphConf(
            std::string const& dbDirPath, std::string const& dbName);

        static void purge();

        static void scanTweets(
            std::string const & dirPath,
            std::function<void (uint64_t time, int64_t from,
                                std::vector<int64_t> const& tos,
                                Tweet const& tweet)> visitor,
            uint64_t& tsStart,
            uint64_t& tsEnd);

        static void populateGraphFromTweets(
            std::string const & dirPath,
            std::vector< std::unique_ptr<core::InteractionGraph> > & graphs,
            uint64_t& tsStart,
            uint64_t& tsEnd,
            std::unordered_set<int64_t> & vertices);

        static std::vector<core::FocusedIntervalQuery> genSearchQueries(
            std::vector<std::vector<std::string>> const & templates,
            double queryZipfParam,
            int numQueries,
            uint64_t tsStart,
            uint64_t tsEnd,
            double delta,
            core::InteractionGraph * graph,
            bool const useMostProlificVertex=false);

        static double runWorkload(
            core::InteractionGraph * graph,
            std::vector<core::FocusedIntervalQuery> const & queries);

        static double runDFS(
            core::InteractionGraph * graph,
            std::vector<core::FocusedIntervalQuery> const & queries);

        static double runBFS(
            core::InteractionGraph * graph,
            std::vector<core::FocusedIntervalQuery> const & queries);

        static void dfs(
            core::InteractionGraph * graph,
            core::FocusedIntervalQuery const & query,
            std::set<VertexId> & visited );

        static void bfs(
            core::InteractionGraph * graph,
            core::FocusedIntervalQuery const & query );

    private:
        static std::random_device randomDevice_;
        static std::mt19937 randomGen_;
        static std::uniform_int_distribution<int> seedDist_;
    };
} }

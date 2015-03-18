#pragma once

#include <intergdb/core/Conf.h>
#include <intergdb/core/InteractionGraph.h>

#include <string>
#include <vector>

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
    static core::Conf createGraphConf(std::string const& dbDirPath,
                                      std::string const& dbName);
    static void scanTweets(std::string const & dirPath, std::function<
        void (uint64_t time,
              int64_t from, std::vector<int64_t> const& tos,
              Tweet const& tweet)> visitor, uint64_t& tsStart, uint64_t& tsEnd);
    static void populateGraphFromTweets(std::string const & dirPath,
                                        std::vector< std::unique_ptr<core::InteractionGraph> > & graphs, 
                                        uint64_t& tsStart, 
                                        uint64_t& tsEnd,  
                                        std::unordered_set<int64_t> & vertices);
};

} }

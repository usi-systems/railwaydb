#include <intergdb/exp/ExpSetupHelper.h>

using namespace std;
using namespace intergdb::core;
using namespace intergdb::exp;

void printTweets()
{
    ExpSetupHelper::scanTweets("data/tweets", [&] (uint64_t time,
        int64_t from, vector<int64_t> const& tos, Tweet const& tweet)
    {
        for (auto const& to : tos)
            cerr << time << ", " << from << " -> " << to
                 << ", tweet: " << tweet << endl;
    });
}

void createTweetDB()
{
    Conf conf = ExpSetupHelper::createGraphConf("data", "tweetDB");
    InteractionGraph tweetDB(conf);
    ExpSetupHelper::populateGraphFromTweets("data/tweets", tweetDB);
}

int main()
{
    // printTweets();
    createTweetDB();
    return EXIT_SUCCESS;
}

#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExpSetupHelper.h>
#include <intergdb/simulation/Experiments.h>

#include <cstdlib>

using namespace std;
using namespace intergdb;
using namespace intergdb::core;
using namespace intergdb::simulation;

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

int simulation()
{
    (VsStorageOverheadThreshold()).run();
    (VsNumAttributes()).run();
    (VsNumQueryKinds()).run();
    // (VsQueryLength()).run();
    // (VsAttributeSizeSkew()).run();
    // (VsQueryFreqSkew()).run();    
    return EXIT_SUCCESS;
}


int experiment()
{
    // printTweets();
    //createTweetDB();
    (VsBlockSize()).run();
    return EXIT_SUCCESS;
}

int main()
{
    return experiment();
}


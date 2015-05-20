#include <intergdb/simulation/Experiments.h>
#include <intergdb/simulation/ExpSetupHelper.h>

using namespace std;
using namespace intergdb;
using namespace intergdb::simulation;

struct PairHash
{
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
     }
};

void VsInteractionCount::process()
{
    ExperimentalData results("VsInteractionCount");
    results.setDescription("Number of interactions vs. "
                           "number of pairs of vertices CDF");
    results.addField("numInteractions");
    results.addField("numPairsOfVertices");
    results.setKeepValues(false);
    results.open();

    uint64_t startTime = 0;
    uint64_t endTime = 0;

    ExpSetupHelper::scanTweets(
        "data/tweets_all",
        [] (uint64_t, int64_t, vector<int64_t> const&, Tweet const&) {},
        startTime, endTime);

    cout << "start time is " << startTime << endl;
    cout << "end time is " << endTime << endl;
    cout << "num days is " <<
        ((endTime - startTime) / (1. * 24 * 60 * 60 * 1000)) << endl;
    uint64_t stopTime = startTime + UINT64_C(30) * 24 * 60 * 60 * 1000;
    cout << "stop time is " << stopTime << endl;

    unordered_map<std::pair<int64_t,int64_t>, size_t,
                  PairHash> numInteractions;

    ExpSetupHelper::scanTweets("data/tweets_all",
        [&] (uint64_t time, int64_t from,
             vector<int64_t> const& tos, Tweet const& tweet)
        {
            assert(from > 0);
            if (time < stopTime) {
                for (auto const& to : tos) {
                    if (to < 0)
                        continue;
                    auto vpair = make_pair(std::min(from, to),
                                           std::max(from, to));
                    if (numInteractions.count(vpair)==0)
                        numInteractions[vpair] = 1;
                    else
                        numInteractions[vpair]++;
                }
            }
        }, startTime, endTime);

    map<size_t, size_t> numInteractionsToNumPairs;
    for (auto const& pairAndCount : numInteractions)
    {
        if (numInteractionsToNumPairs.count(pairAndCount.second) == 0)
            numInteractionsToNumPairs[pairAndCount.second] = 1;
        else
            numInteractionsToNumPairs[pairAndCount.second]++;
    }

    size_t count = 0;
    for (auto const& nInters2NPairs : numInteractionsToNumPairs)
    {
        count += nInters2NPairs.second;
        results.addRecord();
        results.setFieldValue("numInteractions", nInters2NPairs.first);
        results.setFieldValue("numPairsOfVertices", count);
    }

    results.close();
}

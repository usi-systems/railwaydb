#ifndef INTERGEN_ZIPF_H
#define INTERGEN_ZIPF_H

#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <vector>

namespace intergdb { namespace gen
{
    class Zipf
    {
    private:
        double z_;
        int n_;
        boost::random::mt19937 rng_;
        boost::random::uniform_real_distribution<> uniDist_;
        std::vector<double> p_;
    public:
        Zipf(double z, int n);
        int getRandomValue();
        int getNumItems();
        double getProbability(int i);
    private:
        void init();
    };
} }

#endif /* INTERGEN_ZIPF_H */

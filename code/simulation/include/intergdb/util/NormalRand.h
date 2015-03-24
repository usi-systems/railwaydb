#pragma once

#include <random>
#include <vector>

namespace intergdb { namespace util
{
    class NormalRand
    {
    public:
        NormalRand(double mean, double sigma,
                   double min, double max, size_t n=1024);

        double getRandomValue();

        size_t getNumItems();

        double getProbability(size_t i);

        double getMean();

        void setSeed(unsigned seed);

    private:
        void init();

        size_t getRandomBucket();

        double F(double value);

        double mean_;
        double sigma_;
        double min_;
        double max_;
        double n_;
        double delta_;
        std::unique_ptr<std::mt19937> rng_;
        std::uniform_real_distribution<> uniDist_;
        std::vector<double> p_;
    };
} }

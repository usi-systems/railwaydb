#pragma once

#include <memory>
#include <random>
#include <vector>

namespace intergdb { namespace util
{
    class ZipfRand
    {
    public:
        ZipfRand(double zipfParam, size_t numItems);

        void setSeed(unsigned seed);

        size_t getRandomValue();

        double getItemFrequency(size_t x) const;

    private:
        void init();

        double z_; // zipf param
        size_t n_; // num items
        size_t m_; // num items computed exactly
        const size_t c_; // max items computed exactly
        std::vector<double> p_;
        std::unique_ptr<std::mt19937> rng_;
        std::uniform_real_distribution<> uniDist_;
    };
} }

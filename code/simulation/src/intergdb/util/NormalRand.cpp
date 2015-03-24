#include <intergdb/util/NormalRand.h>

using namespace std;
using namespace intergdb::util;

NormalRand::NormalRand(
    double mean, double sigma, double min, double max, size_t n/*=1024*/)
    : mean_(mean), sigma_(sigma), min_(min), max_(max), n_(n),
    delta_((max-min)/n_), rng_(new std::mt19937()), uniDist_(0,1)
{
    p_.reserve(n);
    init();
}

double NormalRand::F(double value)
{
    return 0.5 * (1+erf((value-mean_)/(sigma_*sqrt(2.0))));
}

void NormalRand::init()
{
    double sum = 0;
    for (size_t i=0; i<n_; i++) {
        double start = min_ + delta_*i;
        double end = start + delta_;
        p_[i] = F(end)-F(start);
        sum = sum + p_[i];
    }
    for (size_t i=0; i<n_; i++)
        p_[i] = p_[i]/sum;
    for (size_t i=1; i<n_; i++)
        p_[i] = p_[i] + p_[i-1];
}

double NormalRand::getRandomValue()
{
    size_t i = getRandomBucket();
    return min_ + delta_*i + uniDist_(*rng_)*delta_;
}

size_t NormalRand::getRandomBucket()
{
    double v = uniDist_(*rng_);
    for(size_t i=0; i<n_; i++) {
        if(v<p_[i])
            return i;
    }
    return 0;
}

double NormalRand::getProbability(size_t i)
{
    if(i==0)
        return p_[i];
    return (p_[i]-p_[i-1]);
}

double NormalRand::getMean()
{
    double mean = 0.0;
    for (size_t i=0; i<n_; ++i) {
        double value = min_ + delta_*i + delta_/2.0;
        double prob = getProbability(i);
        mean += prob*value;
    }
    return mean;
}

void NormalRand::setSeed(unsigned seed)
{
    rng_.reset(new mt19937(seed));
}

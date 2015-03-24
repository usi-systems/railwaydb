#include <intergdb/util/ZipfRand.h>

#include <stdexcept>

using namespace std;
using namespace intergdb;
using namespace intergdb::util;

ZipfRand::ZipfRand(double zipfParam, size_t numItems)
    : z_(zipfParam), n_(numItems), c_(16384),
    rng_(new mt19937()), uniDist_(0,1)
{
    init();
}

void ZipfRand::init()
{
    m_ = ::min(n_, c_);
    p_.reserve(m_);
    double sum = 0;
    for (size_t i=0; i<n_; ++i) {
        double pi = 1.0/::pow(i+1.0, z_);
        if (i<m_)
            p_.push_back(pi);
        sum = sum + pi;
    }
    for (size_t i=0; i<m_; ++i)
        p_[i] = p_[i]/sum;
    for (size_t i=1; i<m_; ++i)
        p_[i] = p_[i] + p_[i-1];
}

size_t ZipfRand::getRandomValue()
{
    double v = uniDist_(*rng_);
    if (v<p_[m_-1])
        return upper_bound(p_.begin(), p_.end(), v)-p_.begin();
    double diff = v - p_[m_-1];
    size_t k = (n_-m_) * (diff / (1.0-p_[m_-1]));
    return m_ + k;
}

double ZipfRand::getItemFrequency(size_t x) const
{
    if(x==0)
        return p_[x];
    if (x<m_)
        return (p_[x]-p_[x-1]);
    return  (1.0-p_[m_-1]) / (n_-m_);
}

void ZipfRand::setSeed(unsigned seed)
{
    rng_.reset(new mt19937(seed));
}

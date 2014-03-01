#include <intergdb/gen/Zipf.h>

using namespace std;
using namespace intergdb::gen;

Zipf::Zipf(double z, int n)
	: z_(z), n_(n), uniDist_(0,1)
{
    rng_.seed(rng_.default_seed);
	p_.reserve(n);
	init();
}

void Zipf::init()
{
	double sum = 0;
	for (int i=0; i<n_; i++){
		p_[i] = 1.0 / pow(i+1.0, z_);
		sum = sum + p_[i];
	}
	for (int i=0; i<n_; i++)
		p_[i] = p_[i]/sum;
	for (int i=1; i<n_; i++)
		p_[i] = p_[i] + p_[i-1];
}

int Zipf::getRandomValue()
{
	double v = uniDist_(rng_);
	for(int i=0; i<n_; i++){
		if(v<p_[i])
			return i;
    }
    return 0;
}

double Zipf::getProbability(int i)
{
	if(i==0)
		return p_[i];
    return (p_[i]-p_[i-1]);
}

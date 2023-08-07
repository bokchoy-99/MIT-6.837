#include "sampler.h"
#include "drand.h"
#include "vectors.h"

Sampler *Sampler::GetSamplerByStrategy(const std::string &strategyString, int nSamples)
{
    if (strategyString == "-random_samples")
    {
        return new RandomSampler();
    }
    else if (strategyString == "-uniform_samples")
    {
        return new UniformSampler(nSamples);
    }
    else if (strategyString == "-jittered_samples")
    {
        return new JitteredSampler(nSamples);
    }
    assert(0);
    return nullptr;
}

Sampler *Sampler::GetDefaultSampler()
{
    return new UniformSampler(1);
}

Vec2f RandomSampler::getSamplePosition(int n)
{
    return Vec2f(static_cast<float>(drand48()), static_cast<float>(drand48()));
}

UniformSampler::UniformSampler(int _nSamples)
{
    assert(int(sqrtf(_nSamples)) * int(sqrtf(_nSamples)) == int(roundf(_nSamples)));
    nSamples = _nSamples;
    alignedStep = int(sqrtf(_nSamples));
}

Vec2f UniformSampler::getSamplePosition(int n)
{
    int x = n % alignedStep, y = n / alignedStep;
    float xPos = 1.f / alignedStep * (x + 0.5f), yPos = 1.f / alignedStep * (y + 0.5f);
    return Vec2f(xPos, yPos);
}

Vec2f JitteredSampler::getSamplePosition(int n)
{
    auto uniformPos = UniformSampler::getSamplePosition(n);
    auto turbulence = Vec2f(static_cast<float>(drand48()) - 0.5f, static_cast<float>(drand48()) - 0.5f) / float(alignedStep);
    auto jitteredPos = uniformPos + turbulence;
    jitteredPos.Clamp();
    return jitteredPos;
}

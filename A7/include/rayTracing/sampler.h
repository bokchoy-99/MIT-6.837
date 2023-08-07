
#ifndef _SAMPLER_H
#define _SAMPLER_H

#include <memory>
#include <string>
#include <cmath>
#include <cassert>

#include "vectors.h"

class Sampler
{
protected:
    int nSamples = 0;

public:
    static Sampler *GetSamplerByStrategy(const std::string &strategyString, int nSamples);
    static Sampler *GetDefaultSampler();
    // 这个得到的采样点坐标，是在一个像素点中的偏移量，在[0, 1]之间 
    virtual Vec2f getSamplePosition(int n) = 0;
};

class RandomSampler : public Sampler
{
public:
    Vec2f getSamplePosition(int n) override;
};

class UniformSampler : public Sampler
{
protected:
    int alignedStep;

public:
    explicit UniformSampler(int _nSamples);

    Vec2f getSamplePosition(int n) override;
};

class JitteredSampler : public UniformSampler
{
public:
    explicit JitteredSampler(int _nSamples) : UniformSampler(_nSamples) {}

    Vec2f getSamplePosition(int n) override;
};

#endif // _SAMPLER_H

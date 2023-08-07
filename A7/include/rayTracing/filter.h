
#ifndef _FILTER_H
#define _FILTER_H

#include <string>
#include <memory>
#include "vectors.h"
#include "film.h"

class Filter
{
public:
    static Filter *GetFilterByStrategy(const string &strategyString, float filterArgument);
    static Filter *GetDefaultFilter();
    Vec3f getColor(int i, int j, Film *film);

    virtual float getWeight(float x, float y) = 0;
    virtual int getSupportRadius() = 0;
};

class BoxFilter : public Filter
{
    float radius;

public:
    explicit BoxFilter(float _radius) : radius(_radius) {}

    float getWeight(float x, float y) override;
    int getSupportRadius() override;
};

class TentFilter : public Filter
{
    float radius;

public:
    explicit TentFilter(float _radius) : radius(_radius) {}

    float getWeight(float x, float y) override;
    int getSupportRadius() override;
};

class GaussianFilter : public Filter
{
    float sigma;

public:
    explicit GaussianFilter(float _sigma) : sigma(_sigma) {}

    float getWeight(float x, float y) override;
    int getSupportRadius() override;
};

#endif // _FILTER_H

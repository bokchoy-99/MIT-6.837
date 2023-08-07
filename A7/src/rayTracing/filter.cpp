
#include "filter.h"

static float eps = 1e-5f;

Filter *Filter::GetFilterByStrategy(const std::string &strategyString, float filterArgument)
{
    if (strategyString == "-box_filter")
    {
        return new BoxFilter(filterArgument);
    }
    else if (strategyString == "-tent_filter")
    {
        return new TentFilter(filterArgument);
    }
    else if (strategyString == "-gaussian_filter")
    {
        return new GaussianFilter(filterArgument);
    }
    assert(0);
    return nullptr;
}

Filter *Filter::GetDefaultFilter()
{
    return new BoxFilter(eps);
}

Vec3f Filter::getColor(int i, int j, Film *film)
{
    // 在 (i,j) 像素点的半径 radius 内，求加权平均（也可以看成卷积）
    Vec3f color;

    float sumWeight = 0.f;

    int supportRadius = getSupportRadius();
    for (int xOffset = -supportRadius; xOffset <= supportRadius; ++xOffset)
    {
        for (int yOffset = -supportRadius; yOffset <= supportRadius; ++yOffset)
        {
            // 当前像素点坐标
            int x = xOffset + i, y = yOffset + j;
            if (x < 0 || x >= film->getWidth() || y < 0 || y >= film->getHeight())
                continue;
            for (int n = 0; n < film->getNumSamples(); ++n)
            {
                auto sample = film->getSample(x, y, n);
                // samplePos 表示 采样点在一个像素内的偏移量
                auto samplePos = sample.getPosition();
                // 没懂，这 -0.5 在干啥，试了下，好像没改变
                // 但是看 film.cpp 里renderFilter的函数，似乎是将一个像素内的中心点作为原点
                // (i + xOffset) + samplePos.x() - (i + 0.5f) 
                // 半径范围内的像素坐标 + 采样点在像素内的偏移量 - 初始像素的中心点 
                auto pixelPos = Vec2f(samplePos.x() + xOffset - 0.5f, samplePos.y() + yOffset - 0.5f);
                auto weight = getWeight(pixelPos.x(), pixelPos.y());
                sumWeight += weight;
                color += sample.getColor() * weight;
            }
        }
    }
    float normalizeFactor = 1.f / sumWeight;
    return color * normalizeFactor;
}

float BoxFilter::getWeight(float x, float y)
{
    return fabsf(x) < radius && fabsf(y) < radius;
}

int BoxFilter::getSupportRadius()
{
    return int(ceilf(radius - 0.5f));
}

float TentFilter::getWeight(float x, float y)
{
    float dist = sqrtf(x * x + y * y);
    return std::max(0.f, 1.f - dist / radius);
}

int TentFilter::getSupportRadius()
{
    return int(ceilf(radius - 0.5f));
}

float GaussianFilter::getWeight(float x, float y)
{
    float dist = sqrtf(x * x + y * y);
    return expf((-dist * dist) / (2.f * sigma * sigma));
}

int GaussianFilter::getSupportRadius()
{
    return int(ceilf(2 * sigma - 0.5f));
}

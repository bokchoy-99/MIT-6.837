#include "spline.h"

Spline::Spline(int _nVertices)
{
    nVertices = nVertices;
    ctrlPoints.resize(static_cast<unsigned long>(nVertices));
}

void Spline::set(int i, const Vec3f &v)
{
    ctrlPoints[i] = v;
}

void Spline::moveControlPoint(int selectedPoint, float x, float y)
{
    set(selectedPoint, Vec3f(x, y, 0.f));
}

void Spline::addControlPoint(int selectedPoint, float x, float y)
{
    ++nVertices;
    ctrlPoints.resize(static_cast<unsigned long>(nVertices));
    for (int i = nVertices - 1; i > selectedPoint; --i)
    {
        ctrlPoints[i] = ctrlPoints[i - 1];
    }
    set(selectedPoint, Vec3f(x, y, 0.f));
}

void Spline::deleteControlPoint(int selectedPoint)
{
    --nVertices;
    for (int i = selectedPoint; i < nVertices - 1; ++i)
    {
        ctrlPoints[i] = ctrlPoints[i + 1];
    }
    ctrlPoints.resize(static_cast<unsigned long>(nVertices));
}

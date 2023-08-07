
#include "sphere.h"
#include "ray.h"
#include "hit.h"
#include <cmath>

Sphere::Sphere(Vec3f _center, float _radius, Material *_material)
{
    material = _material;
    center = _center;
    radius = _radius;
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    const auto &origin = r.getOrigin();
    auto dOrigin = origin - center;
    auto squareLenOrigin = dOrigin.Dot3(dOrigin);
    auto squareRadius = radius * radius;
    auto dFootPointToOrigin = -dOrigin.Dot3(r.getDirection());
    auto squareDistRay = squareLenOrigin - dFootPointToOrigin * dFootPointToOrigin;
    if (squareDistRay > squareRadius)
    {
        return false;
    }
    // 计算两个交点，并判断哪个是近点
    auto squareDistIntersectionToFootPoint = squareRadius - squareDistRay;
    auto dIntersectionToOrigin_1 = dFootPointToOrigin + sqrt(squareDistIntersectionToFootPoint);
    auto dIntersectionToOrigin_2 = dFootPointToOrigin - sqrt(squareDistIntersectionToFootPoint);
    auto dIntersectionToOriginMin = std::min(dIntersectionToOrigin_1, dIntersectionToOrigin_2);
    auto dIntersectionToOriginMax = std::max(dIntersectionToOrigin_1, dIntersectionToOrigin_2);
    float t;
    if (dIntersectionToOriginMin >= tmin)
    {
        t = dIntersectionToOriginMin;
    }
    else if (dIntersectionToOriginMax >= tmin)
    {
        t = dIntersectionToOriginMax;
    }
    else
    {
        return false;
    }
    auto intersection = r.pointAtParameter(t);
    auto normal = intersection - center;
    normal.Normalize();
    h.set(t, material, normal, r);
    return true;
}

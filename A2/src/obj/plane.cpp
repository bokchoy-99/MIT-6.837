#include "plane.h"
#include "ray.h"
#include "hit.h"

Plane::Plane(Vec3f &_normal, float _offset, Material *_material)
{
    normal = _normal;
    normal.Normalize();
    offset = _offset;
    material = _material;
}

Plane::Plane(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, Material *_material)
{
    Vec3f::Cross3(normal, v1 - v0, v2 - v1);
    normal.Normalize();
    offset = normal.Dot3(v0);
    material = _material;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    Hit hit;
    auto nRd = normal.Dot3(r.getDirection());
    if (fabsf(nRd) < 0)
    {
        return false;
    }
    float t = (offset - normal.Dot3(r.getOrigin())) / nRd;
    hit.set(t, material, normal, r);
    if (t >= tmin)
    {
        h = hit;
        return true;
    }
    return false;
}

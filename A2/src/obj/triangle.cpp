#include "triangle.h"
#include "ray.h"
#include "hit.h"

Triangle::Triangle(Vec3f &_v0, Vec3f &_v1, Vec3f &_v2, Material *_material) : plane(_v0, _v1, _v2, _material)
{
    material = _material;
    v0 = _v0;
    v1 = _v1;
    v2 = _v2;

    Vec3f::Cross3(normal, v1 - v0, v2 - v1);
    normal.Normalize();
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f c, edge, pv, intersection;
    Hit hit;
    if (plane.intersect(r, hit, tmin))
    {
        if (hit.getT() >= tmin)
        {
            // inside the tri ?
            intersection = hit.getIntersectionPoint();
            edge = v0 - v1;
            pv = intersection - v0;
            Vec3f::Cross3(c, pv, edge);
            if (c.Dot3(normal) < 0)
                return false;
            edge = v1 - v2;
            pv = intersection - v1;
            Vec3f::Cross3(c, pv, edge);
            if (c.Dot3(normal) < 0)
                return false;
            edge = v2 - v0;
            pv = intersection - v2;
            Vec3f::Cross3(c, pv, edge);
            if (c.Dot3(normal) < 0)
                return false;
            h = hit;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
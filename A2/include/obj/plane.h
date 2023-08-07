#ifndef _PLANE_H
#define _PLANE_H

#include "object3d.h"
#include "vectors.h"

class Plane : public Object3D
{
public:
    Plane(Vec3f &_normal, float _offset, Material *_material);
    Plane(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, Material *_material);

    bool intersect(const Ray &r, Hit &h, float tmin);

private:
    Vec3f normal;
    float offset;
};

#endif

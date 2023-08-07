#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "object3d.h"
#include "vectors.h"
#include "plane.h"

class Triangle : public Object3D
{
public:
    Triangle(Vec3f &_v0, Vec3f &_v1, Vec3f &_v2, Material *_material);

    bool intersect(const Ray &r, Hit &h, float tmin);

private:
    Vec3f v0, v1, v2;
    Vec3f normal;
    Plane plane;
};
#endif
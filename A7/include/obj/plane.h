#ifndef _PLANE_H
#define _PLANE_H

#include "object3d.h"
#include "ray.h"
#include "hit.h"
#include "material.h"

// 平面：法线向量+平面距离远点的距离
class Plane : public Object3D
{
public:
    // CONSTRUCTOR
    Plane(Vec3f &_normal, float _offset, Material *_material);
    Plane(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, Material *_material);

    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint();

    void insertIntoGrid(Grid *g, Matrix *m);

private:
    Vec3f normal;
    float offset;
};
#endif
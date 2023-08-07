#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "object3d.h"
#include "vectors.h"
#include "plane.h"

// 平面：法线向量+平面距离远点的距离
class Triangle : public Object3D
{
public:
    // CONSTRUCTOR
    Triangle(Vec3f &_v0, Vec3f &_v1, Vec3f &_v2, Material *_material);
    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint();
    
private:
    Vec3f v0, v1, v2;
    Vec3f normal;
    Plane plane;
};
#endif
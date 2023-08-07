#ifndef _SPHERE_H
#define _SPHERE_H

#include "object3d.h"
#include "vectors.h"

class Sphere : public Object3D
{
public:
    Sphere(Vec3f _center, float _radius, Material *_material);

    bool intersect(const Ray& r, Hit &h, float tmin);

private:
    Vec3f center;
    float radius;
};


#endif
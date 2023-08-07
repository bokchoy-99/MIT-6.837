#ifndef _SPHERE_H
#define _SPHERE_H

#include "object3d.h"
#include "vectors.h"
#include "boundingbox.h"
#include "ray.h"
#include "hit.h"
#include "grid.h"

// 球体： 中心点 + 半径
class Sphere : public Object3D
{
public:
    // CONSTRUCTOR
    Sphere(Vec3f _center, float _radius, Material *_material);

    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint();
    void insertIntoGrid(Grid *g, Matrix *m);

    void getXYZBySphereCoord(float theta, float phi, float &x, float &y, float &z);

private:
    Vec3f center;
    float radius;
};
#endif
#ifndef _OBJECT3D_H
#define _OBJECT3D_H

#include <iostream>
using namespace std;

#include "vectors.h"

class Ray;
class Hit;
class Material;

// ====================================================================
// ====================================================================

static float eps = 1e-6f;

class Object3D
{
public:
    Object3D() = default;
    ~Object3D() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

protected:
    Material *material = nullptr;
};

// ====================================================================
// ====================================================================

#endif
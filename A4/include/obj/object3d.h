#ifndef _OBJECT3D_H
#define _OBJECT3D_H

#include <iostream>
using namespace std;

#include "vectors.h"
#include "material.h"
#include "ray.h"
#include "hit.h"
#include <GL/gl.h>
#include <GL/glut.h>

// ====================================================================
// ====================================================================

// Ray class mostly copied from Peter Shirley and Keith Morley

class Object3D
{

public:
    Object3D() = default;
    ~Object3D() {}
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual void paint(void) = 0;

protected:
    Material *material = nullptr;
};

// ====================================================================
// ====================================================================

#endif
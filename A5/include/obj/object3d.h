#ifndef _OBJECT3D_H
#define _OBJECT3D_H

#include <iostream>
#include <memory>

#include "vectors.h"
#include "matrix.h"
#include <GL/gl.h>
#include <GL/glut.h>

class Grid;
class Ray;
class Hit;
class Material;
class BoundingBox;

// ====================================================================
// ====================================================================

enum ObjectType
{
    SphereObject,
    PlaneObject,
    TriangleObject,
    GridObject,
    GroupObject
};

class Object3D
{
public:
    Object3D() = default;
    ~Object3D() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual void paint(void) = 0;

    ObjectType getObjectType() const { return type; }
    
    BoundingBox *getBoundingBox() const { return boundingBox; }
    virtual void insertIntoGrid(Grid *g, Matrix *m);

protected:
    Material *material = nullptr;
    BoundingBox *boundingBox = nullptr;
    ObjectType type;
};

// ====================================================================
// ====================================================================

#endif
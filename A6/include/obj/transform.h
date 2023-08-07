#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix.h"
#include "object3d.h"
#include "grid.h"

class Transform : public Object3D
{
public:
    Transform(const Matrix &_mat, Object3D *_object);
    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint();

    void insertIntoGrid(Grid *g, Matrix *m) override;

    static BoundingBox TransformBoundingBox(const Object3D &object, const Matrix &m);
    
    void setFlattened() { flattened = true; }

    ~Transform();
    
private:
    Matrix mat, invMat, invTpMat;
    Object3D *object;
    bool flattened = false;
};
#endif
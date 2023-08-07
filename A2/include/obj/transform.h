#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "object3d.h"
#include "matrix.h"

class Transform : public Object3D
{
public:
    Transform(const Matrix &_mat, Object3D *_object);
    bool intersect(const Ray &r, Hit &h, float tmin);

private:
    Matrix mat, invMat, invTpMat;
    Object3D *object;
};

#endif
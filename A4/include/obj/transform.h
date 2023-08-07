#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix.h"
#include "object3d.h"

class Transform : public Object3D
{
public:
    Transform(const Matrix &_mat, Object3D *_object);
    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint();
    
private:
    Matrix mat, invMat, invTpMat;
    Object3D *object;
};
#endif
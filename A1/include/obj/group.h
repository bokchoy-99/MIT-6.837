#ifndef _GROUP_H
#define _GROUP_H

#include "object3d.h"
#include "vectors.h"

class Group : public Object3D
{
public:
    Group(int _numObjects);

    bool intersect(const Ray &r, Hit &h, float tmin);
    void addObject(int index, Object3D *obj);

private:
    Object3D **objList;
    int numObjects = 0;
};

#endif
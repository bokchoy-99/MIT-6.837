#ifndef _GROUP_H
#define _GROUP_H

#include "object3d.h"
#include "hit.h"
#include "vectors.h"
#include "boundingbox.h"

// 场景描述
class Group : public Object3D
{
public:
    // CONSTRUCTOR
    Group(int _numObjects);
    ~Group() {}
    // 判断是否相交，给定一束光线，通过遍历场景中所有的物体，计算交点并找到最近的交点
    bool intersect(const Ray &r, Hit &h, float tmin);
    void addObject(int index, Object3D *obj);
    void paint();

    void insertIntoGrid(Grid *g, Matrix *m);

private:
    Object3D **objList;
    int numObjects;
};
#endif
#include "group.h"
#include "hit.h"

Group::Group(int _numObjects)
{
    numObjects = _numObjects;
    objList = new Object3D *[numObjects];
}

bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    h = Hit();
    bool success = false;
    // 遍历group中所有的对象并求交点，找到最近的交点
    for (int i = 0; i < numObjects; i++)
    {
        auto obj = objList[i];
        Hit curHit;
        if (obj->intersect(r, curHit, tmin))
        {
            success = true;
            if (curHit.getT() < h.getT())
            {
                h = curHit;
            }
        }
    }
    return success;
}

void Group::addObject(int index, Object3D *obj)
{
    assert(index < numObjects);
    objList[index] = obj;
}
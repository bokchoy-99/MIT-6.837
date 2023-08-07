
#include "group.h"
#include "limits"

Group::Group(int _numObjects) : numObjects(_numObjects)
{
    type = ObjectType::GroupObject;
    objList = new Object3D *[numObjects];

    float maxf = std::numeric_limits<float>::max(), minf = std::numeric_limits<float>::lowest();
    boundingBox = new BoundingBox(Vec3f(maxf, maxf, maxf), Vec3f(minf, minf, minf));
}

bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    bool success = false;
    // 遍历所有的物体，找到最近的交点
    for (int i = 0; i < numObjects; i++)
    {
        Object3D *obj = objList[i];
        Hit currentHit;
        if (obj->intersect(r, currentHit, tmin))
        {
            success = true;
            if (currentHit.getT() < h.getT())
            {
                h = currentHit;
            }
        }
    }
    return success;
}

void Group::addObject(int index, Object3D *obj)
{
    assert(index < numObjects);
    objList[index] = obj;
    // discard those infinite bounding boxes such as planes
    if (obj->getBoundingBox())
    {
        boundingBox->Extend(obj->getBoundingBox());
        // boundingBox->Print();
    }
}

void Group::paint()
{
    for (int i = 0; i < numObjects; ++i)
    {
        objList[i]->paint();
    }
}

void Group::insertIntoGrid(Grid *g, Matrix *m)
{
    for (int i = 0; i < numObjects; i++)
    {
        objList[i]->insertIntoGrid(g, m);
    }
}
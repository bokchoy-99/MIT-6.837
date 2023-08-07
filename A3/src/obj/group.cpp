
#include "group.h"
#include "limits"

bool Group::intersect(const Ray &r, Hit &h, float tmin) {
    h = Hit(std::numeric_limits<float>::max(), nullptr, Vec3f());
    bool success = false;
    
    // 遍历所有的物体，找到最近的交点
    for (int i = 0; i < numObjects; i++) {
        Object3D *obj = objList[i];
        Hit currentHit;
        if (obj->intersect(r, currentHit, tmin)) {
            success = true;
            if (currentHit.getT() < h.getT()) {
                h = currentHit;
            }
        }
    }
    return success;
}

void Group::addObject(int index, Object3D *obj) {
    assert(index < numObjects);
    objList[index] = obj;
}

void Group::paint() {
    for (int i = 0; i < numObjects; ++i) {
        objList[i]->paint();
    }
}

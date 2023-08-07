#include "transform.h"

Transform::Transform(const Matrix &_mat, Object3D *_object) {
    mat = _mat;
    invMat = mat;
    int r = invMat.Inverse();
    assert(r);
    invTpMat = invMat;
    invTpMat.Transpose();
    object = _object;
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin) {
    // 先将 ray 从 world space 转换到 object space
    // transform ray direction from world space to object space
    Vec4f hRayDir(r.getDirection(), 0.f);
    Vec4f hTransformedRayDir = invMat * hRayDir;
    Vec3f transformedRayDir = hTransformedRayDir.xyz();
    float transformedRayDirLength = transformedRayDir.Length();
    transformedRayDir.Normalize();
    // transform ray origin from world space to object space
    Vec4f hRayOrigin(r.getOrigin(), 1.f);
    Vec4f hTransformedRayOrigin = invMat * hRayOrigin;
    hTransformedRayOrigin.DivideByW();
    Vec3f transformedRayOrigin = hTransformedRayOrigin.xyz();
    // 得到 object space 下的 ray ： transformedRay
    Ray transformedRay(transformedRayOrigin, transformedRayDir);
    Hit osHit;
    if (object->intersect(transformedRay, osHit, tmin)) {
        // 将 object space 下 交点的法线 转换到 world space，得到 wsNorm
        auto osNorm = osHit.getNormal();
        auto hOsNorm = Vec4f(osNorm, 0.f);
        auto hWsNorm = invTpMat * hOsNorm;
        auto wsNorm = hWsNorm.xyz();
        wsNorm.Normalize();
        // 由于坐标系的改变，转换到world space的ray的方向向量实际上不一定为 1 ，前面因计算需要，标准化过
        // 而计算得到的 object space 下的ray，方向向量是单位向量
        // 要按照 world space 下的 ray 的 方向向量长度计算交点的 t 
        float osT = osHit.getT();
        float wsT = osT / transformedRayDirLength;
        h = Hit(wsT, osHit.getMaterial(), wsNorm);
        return true;
    } else {
        return false;
    };
}

void Transform::paint() {
    glPushMatrix();
    GLfloat *glMatrix = mat.glGet();
    glMultMatrixf(glMatrix);
    delete[] glMatrix;
    object->paint();
    glPopMatrix();
}

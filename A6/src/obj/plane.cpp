#include "plane.h"
#include "transform.h"
#include "raytracing_stats.h"
#define MY_INF 1e6

Plane::Plane(Vec3f &_normal, float _offset, Material *_material)
{
    type = ObjectType::PlaneObject;
    normal = _normal;
    offset = _offset;
    material = _material;
}

Plane::Plane(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, Material *_material)
{
    type = ObjectType::PlaneObject;
    Vec3f::Cross3(normal, v1 - v0, v2 - v1);
    normal.Normalize();
    offset = normal.Dot3(v0);
    material = _material;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    RayTracingStats::IncrementNumIntersections();
    // 标准化方向向量
    Vec3f dir_nor = r.getDirection();
    float dir_len = dir_nor.Length();
    dir_nor.Normalize();

    if (dir_nor.Dot3(normal) == 0)
        return false; // 光线方向与平面平行
    float t = (offset - r.getOrigin().Dot3(normal)) / (dir_nor.Dot3(normal));
    if (t < 0)
        return false; // 在光线背面
    // 与射线相交
    if (t / dir_len < tmin)
        return false;
    if (h.getMaterial() != NULL && h.getT() < t / dir_len)
        return true;
    h.set(t / dir_len, material, normal, r, ObjectType::PlaneObject);
    return true;
}

void Plane::insertIntoGrid(Grid *g, Matrix *m)
{
    auto *copy = new Plane(*this);
    if (m == nullptr)
    {
        g->infiniteObjects.emplace_back(copy);
    }
    else
    {
        auto transformObject = new Transform(*m, copy);
        g->infiniteObjects.emplace_back(transformObject);
    }
}

void Plane::paint()
{
    material->glSetMaterial();

    Vec3f v;
    if (fabsf(normal.y()) < 1e-3 && fabsf(normal.z()) < 1e-3)
    {
        v.Set(0.f, 1.f, 0.f);
    }
    else
    {
        v.Set(1.f, 0.f, 0.f);
    }
    Vec3f b1, b2, b3, b4;
    Vec3f::Cross3(b1, v, normal);
    Vec3f::Cross3(b2, normal, b1);

    float big = 1e4;

    b1.Normalize();
    b3 = b1;
    b3.Negate();
    b1 *= big;
    b1 += normal * offset;
    b3 *= big;
    b3 += normal * offset;

    b2.Normalize();
    b4 = b2;
    b4.Negate();
    b2 *= big;
    b2 += normal * offset;
    b4 *= big;
    b4 += normal * offset;

    GLfloat x, y, z;

    glBegin(GL_QUADS);
    
    normal.Get(x, y, z);
    glNormal3f(x, y, z);
    b1.Get(x, y, z);
    glVertex3f(x, y, z);
    b2.Get(x, y, z);
    glVertex3f(x, y, z);
    b3.Get(x, y, z);
    glVertex3f(x, y, z);
    b4.Get(x, y, z);
    glVertex3f(x, y, z);

    glEnd();
}
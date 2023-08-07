#include "plane.h"

#define MY_INF 1e6

Plane::Plane(Vec3f &_normal, float _offset, Material *_material)
{
    normal = _normal;
    offset = _offset;
    material = _material;
}

Plane::Plane(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2, Material *_material)
{
    Vec3f::Cross3(normal, v1 - v0, v2 - v1);
    normal.Normalize();
    offset = normal.Dot3(v0);
    material = _material;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    Hit hit;
    auto nRd = normal.Dot3(r.getDirection());
    if (fabsf(nRd) < 0) {
        // vertical
        return false;
    }
    float t = (offset - normal.Dot3(r.getOrigin())) / nRd;
    hit.set(t, material, normal, r);
    if (t >= tmin) {
        h = hit;
        return true;
    }
    return false;
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
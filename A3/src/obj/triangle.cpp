#include "triangle.h"

Triangle::Triangle(Vec3f &_v0, Vec3f &_v1, Vec3f &_v2, Material *_material)
{
    material = _material;
    v0 = _v0;
    v1 = _v1;
    v2 = _v2;

    plane = Plane(v0, v1, v2, material);

    Vec3f::Cross3(normal, v1 - v0, v2 - v1);
    normal.Normalize();
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
    Vec3f c, edge, pv, intersection;
    Hit hit;
    if (plane.intersect(r, hit, tmin)) {
        if (hit.getT() >= tmin) {
            // test inside the tri
            intersection = hit.getIntersectionPoint();
            edge = v0 - v1;
            pv = intersection - v0;
            Vec3f::Cross3(c, pv, edge);
            if (c.Dot3(normal) < 0)
                return false;
            edge = v1 - v2;
            pv = intersection - v1;
            Vec3f::Cross3(c, pv, edge);
            if (c.Dot3(normal) < 0)
                return false;
            edge = v2 - v0;
            pv = intersection - v2;
            Vec3f::Cross3(c, pv, edge);
            if (c.Dot3(normal) < 0)
                return false;
            h = hit;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void Triangle::paint()
{
    GLfloat x, y, z;

    material->glSetMaterial();

    glBegin(GL_TRIANGLES);
    normal.Get(x, y, z);
    glNormal3f(x, y, z);
    v0.Get(x, y, z);
    glVertex3f(x, y, z);
    v1.Get(x, y, z);
    glVertex3f(x, y, z);
    v2.Get(x, y, z);
    glVertex3f(x, y, z);
    glEnd();
}

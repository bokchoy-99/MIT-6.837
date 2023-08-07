
#include "sphere.h"

#include <cmath>

static float eps = 1e-5f;

Sphere::Sphere(Vec3f _center, float _radius, Material *_material)
{
    type = ObjectType::SphereObject;
    center = _center;
    radius = _radius;
    material = _material;

    Vec3f boundingRadius(radius, radius, radius);
    Vec3f boundingMin = center - boundingRadius, boundingMax = center + boundingRadius;
    boundingBox = new BoundingBox(boundingMin, boundingMax);
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    // 判断是否有相交
    const auto &origin = r.getOrigin();
    auto dOrigin = origin - center;
    auto squareLenOrigin = dOrigin.Dot3(dOrigin);
    auto squareRadius = radius * radius;
    auto dFootPointToOrigin = -dOrigin.Dot3(r.getDirection());
    auto squareDistRay = squareLenOrigin - dFootPointToOrigin * dFootPointToOrigin;
    if (squareDistRay > squareRadius)
    {
        // no hit
        return false;
    }
    // 计算两个交点，并判断哪个是近点
    auto squareDistIntersectionToFootPoint = squareRadius - squareDistRay;
    auto dIntersectionToOrigin_1 = dFootPointToOrigin + sqrt(squareDistIntersectionToFootPoint);
    auto dIntersectionToOrigin_2 = dFootPointToOrigin - sqrt(squareDistIntersectionToFootPoint);
    auto dIntersectionToOriginMin = std::min(dIntersectionToOrigin_1, dIntersectionToOrigin_2);
    auto dIntersectionToOriginMax = std::max(dIntersectionToOrigin_1, dIntersectionToOrigin_2);
    float t;
    if (dIntersectionToOriginMin >= tmin)
    {
        t = dIntersectionToOriginMin;
    }
    else if (dIntersectionToOriginMax >= tmin)
    {
        t = dIntersectionToOriginMax;
    }
    else
    {
        return false;
    }
    auto intersection = r.pointAtParameter(t);
    auto normal = intersection - center;
    normal.Normalize();
    h.set(t, material, normal, r, ObjectType::SphereObject);
    return true;
}

// 读入的步长
int g_theta_steps = 10, g_phi_steps = 10;
bool g_gouraud = false;

void Sphere::paint()
{
    material->glSetMaterial();
    float pi = acos(-1);
    float theta_step = pi * 2.f / g_theta_steps;
    float phi_step = pi / g_phi_steps;
    float theta = 0.f, phi = 0.f; // phi - 天顶角(竖着的半圆); theta - 方位角（水平的圆）
    float x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3, xn, yn, zn;

    glBegin(GL_QUADS);
    for (int iPhi = 0; iPhi < g_phi_steps; ++iPhi)
    {
        float nextPhi = phi + phi_step;
        for (int iTheta = 0; iTheta < g_theta_steps; ++iTheta)
        {
            float nextTheta = theta + theta_step;
            getXYZBySphereCoord(theta, phi, x0, y0, z0);
            getXYZBySphereCoord(theta, nextPhi, x1, y1, z1);
            getXYZBySphereCoord(nextTheta, nextPhi, x2, y2, z2);
            getXYZBySphereCoord(nextTheta, phi, x3, y3, z3);
            if (!g_gouraud)
            {
                Vec3f v0(x0, y0, z0), v1(x1, y1, z1), v2(x2, y2, z2), v3(x3, y3, z3);
                Vec3f norm = (v0 + v1 + v2 + v3) * (0.25f) - center;
                norm.Get(xn, yn, zn);
                glNormal3f(xn, yn, zn);
            }
            if (g_gouraud)
                glNormal3f(x0 - center.x(), y0 - center.y(), z0 - center.z());
            glVertex3f(x0, y0, z0);
            if (g_gouraud)
                glNormal3f(x1 - center.x(), y1 - center.y(), z1 - center.z());
            glVertex3f(x1, y1, z1);
            if (g_gouraud)
                glNormal3f(x2 - center.x(), y2 - center.y(), z2 - center.z());
            glVertex3f(x2, y2, z2);
            if (g_gouraud)
                glNormal3f(x3 - center.x(), y3 - center.y(), z3 - center.z());
            glVertex3f(x3, y3, z3);

            theta = nextTheta;
        }
        phi = nextPhi;
    }
    glEnd();
}

void Sphere::getXYZBySphereCoord(float theta, float phi, float &x, float &y, float &z)
{
    // The assignment use negative axis-z as forward, positive axis-y as up and positive axis-x as right
    float rsint = radius * sinf(phi);
    float rcost = radius * cosf(phi);
    x = rsint * cosf(theta) + center.x();
    y = rcost + center.y();
    z = -(rsint * sinf(theta)) + center.z();
}

void Sphere::insertIntoGrid(class Grid *g, Matrix *m)
{
    if (m != nullptr)
    {
        Object3D::insertIntoGrid(g, m);
        return;
    }
    auto bbMin = g->getBBMin(), bbMax = g->getBBMax();
    int nx, ny, nz;
    g->getN(nx, ny, nz);
    auto step = g->getStep();
    float gridDiagLen = step.Length() / 2.f;

    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            for (int k = 0; k < nz; ++k)
            {
                float x = i + 0.5f, y = j + 0.5f, z = k + 0.5f;
                Vec3f gridCenter = bbMin + step * Vec3f(x, y, z);
                float dist = (gridCenter - center).Length();
                if (dist < radius + gridDiagLen + eps)
                    g->cells[i][j][k].emplace_back(this);
            }
        }
    }
}
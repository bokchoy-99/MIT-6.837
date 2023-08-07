#include "triangle.h"
#include "raytracing_stats.h"

Triangle::Triangle(Vec3f &_v0, Vec3f &_v1, Vec3f &_v2, Material *_material) : plane(_v0, _v1, _v2, _material)
{
    type = ObjectType::TriangleObject;
    material = _material;
    v0 = _v0;
    v1 = _v1;
    v2 = _v2;

    Vec3f::Cross3(normal, v1 - v0, v2 - v1);
    normal.Normalize();

    float maxf = std::numeric_limits<float>::max(), minf = std::numeric_limits<float>::lowest();
    boundingBox = new BoundingBox(Vec3f(maxf, maxf, maxf), Vec3f(minf, minf, minf));
    boundingBox->Extend(v0);
    boundingBox->Extend(v1);
    boundingBox->Extend(v2);
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
    RayTracingStats::IncrementNumIntersections();
    Vec3f c, edge, pv, intersection;
    Hit hit;
    if (plane.intersect(r, hit, tmin))
    {
        if (hit.getT() >= tmin)
        {
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
        }
        else
        {
            return false;
        }
    }
    else
    {
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

void Triangle::insertIntoGrid(Grid *g, Matrix *m)
{
    if (m != nullptr)
    {
        Object3D::insertIntoGrid(g, m);
        return;
    }

    int iMin[3], iMax[3];
    g->getGridCellIndex(boundingBox->getMin(), iMin);
    g->getGridCellIndex(boundingBox->getMax(), iMax);
    for (int i = iMin[0]; i <= iMax[0]; ++i)
    {
        for (int j = iMin[1]; j <= iMax[1]; ++j)
        {
            for (int k = iMin[2]; k <= iMax[2]; ++k)
            {
                g->cells[i][j][k].emplace_back(this);
            }
        }
    }
}
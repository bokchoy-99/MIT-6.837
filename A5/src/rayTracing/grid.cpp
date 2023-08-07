#include "grid.h"
#include "rayTree.h"
#include "hsl.h"
#include "plane.h"
#include <limits>
#include <functional>
#include <cmath>

static float eps = 1e-5f;
constexpr int Grid::SchemaColorNumber;

PhongMaterial Grid::GridMaterial(Vec3f(1.f, 1.f, 1.f), Vec3f(), 0.f, Vec3f(), Vec3f(), 1.f);

Grid::Grid(BoundingBox *bb, int _nx, int _ny, int _nz)
{
    type = ObjectType::GridObject;

    // 初始化 Grid
    boundingBox = bb;
    nx = _nx;
    ny = _ny;
    nz = _nz;
    bbMin = bb->getMin();
    bbMax = bb->getMax();
    step = (bbMax - bbMin) * Vec3f(1.f / nx, 1.f / ny, 1.f / nz);

    // 初始化cells
    cells.resize(static_cast<unsigned long>(nx));
    for (int i = 0; i < nx; i++)
    {
        cells[i].resize(static_cast<unsigned long>(ny));
        for (int j = 0; j < ny; j++)
        {
            cells[i][j].resize(static_cast<unsigned long>(nz));
            for (int k = 0; k < nz; k++)
            {
                cells[i][j][k].clear();
            }
        }
    }

    material = &GridMaterial;
    // init material schema
    for (int i = 1; i < SchemaColorNumber; ++i)
    {
        HSLColor hsl(360.f - 360.f / (SchemaColorNumber - 1) * (i + 1), 1.f, 0.5f);
        materialSchema[i] = PhongMaterial(hsl.toRGBColor(), Vec3f(), 0.f, Vec3f(), Vec3f(), 1.f);
    }
    materialSchema[0] = GridMaterial;
}

void Grid::getN(int &xx, int &yy, int &zz)
{
    xx = nx;
    yy = ny;
    zz = nz;
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
    initializeRayMarch(r, tmin);
    if (mi.hit)
    {
        mi.hit = false;
        bool firstHit = false;
        float tHit = 0.f;
        Vec3f nHit;
        int nObjects = 0;
        while (mi.index[0] >= 0 && mi.index[0] < nx &&
               mi.index[1] >= 0 && mi.index[1] < ny &&
               mi.index[2] >= 0 && mi.index[2] < nz)
        {
            if (!cells[mi.index[0]][mi.index[1]][mi.index[2]].empty())
            {
                mi.hit = true;
                if (!firstHit)
                {
                    tHit = mi.tmin;
                    nHit = mi.normal_to_cell;
                    nObjects = static_cast<int>(cells[mi.index[0]][mi.index[1]][mi.index[2]].size());
                    firstHit = true;
                }
            }
            mi.NextCell();
            addEnteredCell(mi.index[0], mi.index[1], mi.index[2], mi.next_dir, mi.enteredFacePositive);
        }
        if (mi.hit)
        {
            nHit.Negate();
            h.set(tHit, getCellMaterial(std::min(nObjects, SchemaColorNumber) - 1), nHit, r, ObjectType::GridObject);
            return true;
        }
    }
    return false;
}

void Grid::initializeRayMarch(const Ray &r, float tmin)
{
    mi = MarchingInfo();
    // 初始化 各个方向和步长
    for (int i = 0; i < 3; i++)
    {
        mi.sign[i] = r.getDirection()[i] > 0.f ? 1 : -1;
        mi.d_t[i] = step[i] / r.getDirection()[i] * mi.sign[i];
    }
    // 初始化起始点
    auto startPoint = r.pointAtParameter(tmin);

    // 求交点
    Vec3f hitPoint;
    // 相交面
    int enteredFaceAxis = -1, enteredFacePositive = 0;

    if (startPoint.x() > bbMin.x() && startPoint.x() < bbMax.x() &&
        startPoint.y() > bbMin.y() && startPoint.y() < bbMax.y() &&
        startPoint.z() > bbMin.z() && startPoint.z() < bbMax.z())
    {
        // 在 boundingbox内
        mi.hit = true;
        hitPoint = startPoint;
    }
    else
    {
        // 在boundingbox
        // 构造平面
        Vec3f v[8];
        float x[2] = {bbMin.x(), bbMax.x()};
        float y[2] = {bbMin.y(), bbMax.y()};
        float z[2] = {bbMin.z(), bbMax.z()};
        for (int i = 0; i < 8; i++)
        {
            v[i].SetX(x[(i & 0x4u) >> 2u]);
            v[i].SetY(y[(i & 0x2u) >> 1u]);
            v[i].SetZ(z[(i & 0x1u)]);
        }
        //    1   3
        //    0   2
        // 5   7
        // 4   6
        mi.hit = false;
        Hit h[6];
        bool validHits[6] = {false, false, false, false, false, false};
        Plane planes[6] = {
            Plane(v[0], v[1], v[3], material), // left side
            Plane(v[4], v[5], v[7], material), // right side
            Plane(v[0], v[4], v[5], material), // down side
            Plane(v[2], v[6], v[7], material), // up side
            Plane(v[0], v[4], v[6], material), // back side
            Plane(v[1], v[5], v[7], material), // forward side
        };
        Vec3f planeNormals[6] = {
            Vec3f(1.f, 0.f, 0.f),  // left side
            Vec3f(-1.f, 0.f, 0.f), // right side
            Vec3f(0.f, 1.f, 0.f),  // down side
            Vec3f(0.f, -1.f, 0.f), // up side
            Vec3f(0.f, 0.f, 1.f),  // back side
            Vec3f(0.f, 0.f, -1.f)  // forward side
        };
        Vec3f intersect;
        auto bbContains = [this](const Vec3f &p) -> bool
        {
            return p.x() >= this->bbMin.x() - eps && p.x() <= this->bbMax.x() + eps &&
                   p.y() >= this->bbMin.y() - eps && p.y() <= this->bbMax.y() + eps &&
                   p.z() >= this->bbMin.z() - eps && p.z() <= this->bbMax.z() + eps;
        };
        // 对各个平面求交
        for (int i = 0; i < 6; i++)
        {
            if (planes[i].intersect(r, h[i], tmin))
            {
                intersect = h[i].getIntersectionPoint();
                if (bbContains(intersect))
                {
                    validHits[i] = true;
                    mi.hit = true;
                }
            }
        }
        // 找到最近的交点
        Hit minHit;
        for (int i = 0; i < 6; i++)
        {
            if (validHits[i] && h[i].getT() < minHit.getT())
            {
                minHit = h[i];
                mi.normal_to_cell = planeNormals[i];
                enteredFaceAxis = i / 2;
                enteredFacePositive = i % 2;
            }
        }
        hitPoint = minHit.getIntersectionPoint();
        mi.tmin = minHit.getT();
    }
    // 计算交点处于的cell
    getGridCellIndex(hitPoint, mi.index);
    for (int i = 0; i < 3; i++)
    {
        // 因为 index [0, nx / ny / nz - 1]
        // 所以负方向的下一个交点不需要 -1
        int nextIndex = mi.index[i] + (mi.sign[i] + 1) / 2;
        float nextPos = bbMin[i] + nextIndex * step[i];
        if (r.getDirection()[i] == 0.f)
        {
            mi.t_next[i] = INFINITY;
        }
        else
        {
            mi.t_next[i] = (nextPos - r.getOrigin()[i]) / r.getDirection()[i];
        }
    }
    // 绘制相交平面
    if (mi.hit)
        addEnteredCell(mi.index[0], mi.index[1], mi.index[2], enteredFaceAxis, enteredFacePositive);
}

void Grid::addEnteredCell(int i, int j, int k, int index, int positive)
{
    if (i < 0 || i >= nx ||
        j < 0 || j >= ny ||
        k < 0 || k >= nz)
        return;

    Vec3f v[8];
    for (unsigned int m = 0; m < 8; ++m)
    {
        v[m] = bbMin + step * Vec3f(i + ((m & 0x4u) >> 2u), j + ((m & 0x2u) >> 1u), k + (m & 0x1u));
    }
    Vec3f n[6] = {
        Vec3f(1.f, 0.f, 0.f), // left side
        Vec3f(1.f, 0.f, 0.f), // right side
        Vec3f(0.f, 1.f, 0.f), // down side
        Vec3f(0.f, 1.f, 0.f), // up side
        Vec3f(0.f, 0.f, 1.f), // back side
        Vec3f(0.f, 0.f, 1.f)  // forward side
    };

    // 相交的Cell
    for (int m = 0; m < 6; ++m)
    {
        RayTree::AddHitCellFace(v[sideIndex[m][0]], v[sideIndex[m][1]], v[sideIndex[m][2]], v[sideIndex[m][3]],
                                n[m], getCellMaterial(crtColorIndex % SchemaColorNumber));
    }

    // 相交的面
    if (index != -1)
    {
        int m = index * 2 + positive;
        RayTree::AddEnteredFace(v[sideIndex[m][0]], v[sideIndex[m][1]], v[sideIndex[m][2]], v[sideIndex[m][3]],
                                n[m], getCellMaterial(crtColorIndex % SchemaColorNumber));
    }

    ++crtColorIndex;
}

// 找到点 p 处于的 cell 位置（x,y,z）
void Grid::getGridCellIndex(const Vec3f &p, int index[3])
{
    int n[3] = {nx, ny, nz};
    for (int i = 0; i < 3; ++i)
    {
        float fi = (p[i] - bbMin[i]) / step[i];
        index[i] = std::max(0, std::min(int(floorf(fi)), n[i] - 1));
    }
}

Material *Grid::getCellMaterial(int index) const
{
    return const_cast<PhongMaterial *>(materialSchema.data()) + index;
}

void Grid::paint()
{
    for (unsigned i = 0; i < nx; ++i)
    {
        for (unsigned j = 0; j < ny; ++j)
        {
            for (unsigned k = 0; k < nz; ++k)
            {
                if (!cells[i][j][k].empty())
                {
                    auto nObjects = static_cast<int>(cells[i][j][k].size());
                    materialSchema[std::min(nObjects, SchemaColorNumber) - 1].glSetMaterial();

                    Vec3f v[8];
                    for (unsigned int m = 0; m < 8; ++m)
                    {
                        v[m] = bbMin + step * Vec3f(i + ((m & 0x4u) >> 2u), j + ((m & 0x2u) >> 1u), k + (m & 0x1u));
                    }

                    auto addPointsToGL = [v, this](int i)
                    {
                        for (int m = 0; m < 4; ++m)
                            glVertex3fv(v[this->sideIndex[i][m]].GetData());
                    };

                    glBegin(GL_QUADS);
                    // left side
                    glNormal3f(-1.f, 0.f, 0.f);
                    addPointsToGL(0);
                    // right side
                    glNormal3f(-1.f, 0.f, 0.f);
                    addPointsToGL(1);
                    // down side
                    glNormal3f(0.f, -1.f, 0.f);
                    addPointsToGL(2);
                    // up side
                    glNormal3f(0.f, -1.f, 0.f);
                    addPointsToGL(3);
                    // back side
                    glNormal3f(0.f, 0.f, 1.f);
                    addPointsToGL(4);
                    // forward side
                    glNormal3f(0.f, 0.f, 1.f);
                    addPointsToGL(5);
                    glEnd();
                };
            }
        }
    }
}
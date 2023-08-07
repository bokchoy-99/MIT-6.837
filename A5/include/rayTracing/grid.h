#ifndef _GRID_H
#define _GRID_H

#include <vector>

#include "object3d.h"
#include "boundingbox.h"
#include "material.h"
#include "marchingInfo.h"

class Grid : public Object3D
{
public:
    vector<vector<vector<vector<Object3D *>>>> cells;

    Grid(BoundingBox *bb, int _nx, int _ny, int _nz);
    bool intersect(const Ray &r, Hit &h, float tmin);
    void paint();

    void getN(int &nx, int &ny, int &nz);
    Vec3f getBBMin() { return bbMin; }
    Vec3f getBBMax() { return bbMax; }
    Vec3f getStep() { return step; }
    void getGridCellIndex(const Vec3f &p, int index[3]);

    void refreshColorSchema() { crtColorIndex = 0; }

private:
    static PhongMaterial GridMaterial;

    // 整个 grid 的大小 nx * ny * nz
    int nx, ny, nz;
    // boundingbox的两个顶点
    Vec3f bbMin, bbMax;
    // 每个cell的步长
    Vec3f step;

    MarchingInfo mi;

    void initializeRayMarch(const Ray &r, float tmin);
    void addEnteredCell(int i, int j, int k, int index, int positive);

    int sideIndex[6][4] = {
        {0, 1, 3, 2},
        {4, 5, 7, 6},
        {0, 4, 5, 1},
        {2, 6, 7, 3},
        {0, 4, 6, 2},
        {1, 5, 7, 3}};

    Material *getCellMaterial(int index) const;

    static constexpr int SchemaColorNumber = 17;
    int crtColorIndex = 0;
    array<PhongMaterial, SchemaColorNumber> materialSchema;
};

#endif
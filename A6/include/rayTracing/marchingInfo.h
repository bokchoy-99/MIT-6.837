#ifndef _MARCHING_INFO_H
#define _MARCHING_INFO_H

#include "vectors.h"

class MarchingInfo
{
public:
    MarchingInfo();
    void NextCell();
    
    // 当前光线最近交点的 t
    float tmin;
    // 当前处于的cell的坐标（x, y, z）
    int index[3];
    // t_next表示光线在三个方向上的下一个交点的 t
    // d_t 表示在光线在三个方向上的步长，每走一个cell
    Vec3f t_next, d_t;
    // 光线在三个方向的正负
    int sign[3];
    // 当前相交面的法向量
    Vec3f normal_to_cell;
    // 是否相交
    bool hit = false;
    // 下一个相交网格是哪个方向的 0, 1, 2
    int next_dir;
    // 相交面的正负，该值的计算会与定义的 Plane 的顺序有关
    int enteredFacePositive;
};

#endif
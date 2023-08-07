#include "marchingInfo.h"
#include "raytracing_stats.h"
#include <limits>

MarchingInfo::MarchingInfo()
{
    tmin = 0.f;
    index[0] = index[1] = index[2] = 0;
    t_next = Vec3f(0.f, 0.f, 0.f);
    d_t = Vec3f(0.f, 0.f, 0.f);
    sign[0] = sign[1] = sign[2] = 0;
    normal_to_cell = Vec3f(0.f, 0.f, 0.f);
    hit = false;
    next_dir = -1;
    enteredFacePositive = 0;
}

void MarchingInfo::NextCell()
{
    RayTracingStats::IncrementNumGridCellsTraversed();

    int smallestIndex = -1;
    float smallestT = numeric_limits<float>::max();
    for (int i = 0; i < 3; i++)
    {
        if (t_next[i] >= tmin && t_next[i] < smallestT)
        {
            smallestIndex = i;
            smallestT = t_next[i];
        }
    }
    assert(smallestIndex != -1);
    // 更新 index、tmin、t_next、normal_to_cell
    Vec3f normals[3] = {Vec3f(1.f, 0.f, 0.f), Vec3f(0.f, 1.f, 0.f), Vec3f(0.f, 0.f, 1.f)};
    index[smallestIndex] += sign[smallestIndex];
    tmin = t_next[smallestIndex];
    // t_next 每次更新一个方向即可
    t_next[smallestIndex] += d_t[smallestIndex];
    normal_to_cell = normals[smallestIndex] * sign[smallestIndex];

    // 更新相交面的信息
    next_dir = smallestIndex;
    // sign + 对应 0 2 4 的 plane
    // sign - 对应 1 3 5 的 plane
    // 与定义的 plane 顺序有关
    enteredFacePositive = (1 - sign[smallestIndex]) / 2;
}
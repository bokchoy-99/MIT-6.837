
#include "camera.h"

#include <limits>

OrthographicCamera::OrthographicCamera(const Vec3f &_center, const Vec3f &_direction, const Vec3f &_up, float _size)
{
    center = _center;
    direction = _direction;
    up = _up;
    size = _size;
    direction.Normalize();
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();
    Vec3f::Cross3(up, horizontal, direction);
    up.Normalize();
}

// 传入的point的范围是[0, 1]之间
Ray OrthographicCamera::generateRay(Vec2f point)
{
    // 将图像中的二维点（width，height）对应到相机的（size，size）
    // 图像的(0,0) 对应相机的(-0.5, -0.5) (相机以center为中心点)
    auto origin = center + (point.x() - 0.5f) * size * horizontal + (point.y() - 0.5f) * size * up;
    return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const
{
    return std::numeric_limits<float>::lowest();
}

PerspectiveCamera::PerspectiveCamera(const Vec3f &_center, const Vec3f &_direction, const Vec3f &_up, float _angle)
{
    center = _center;
    direction = _direction;
    up = _up;
    angle = _angle;
    direction.Normalize();
    Vec3f::Cross3(horizontal, direction, up);
    horizontal.Normalize();
    Vec3f::Cross3(up, horizontal, direction);
    up.Normalize();
}

Ray PerspectiveCamera::generateRay(Vec2f point)
{
    Vec3f pos = center;
    Vec3f ray_dir = direction + (point.y() - 0.5) * tan(angle / 2) * 2 * up + (point.x() - 0.5) * tan(angle / 2) * 2 * horizontal;
    ray_dir.Normalize();
    return Ray(pos, ray_dir);
}

float PerspectiveCamera::getTMin() const
{
    return 0;
}

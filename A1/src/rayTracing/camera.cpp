
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
    // 存疑？，x对应hor？还是up？
    auto origin = center + (point.x() - 0.5f) * size * horizontal + (point.y() - 0.5f) * size * up;
    return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const
{
    return std::numeric_limits<float>::lowest();
}

#include "camera.h"

#include <limits>

#include "matrix.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

Ray OrthographicCamera::generateRay(Vec2f point)
{
    // 将图像中的二维点（width，height）对应到相机的（size，size）
    // 图像的(0,0) 对应相机的(-0.5, -0.5) (相机以center为中心点)
    auto origin = center + (point.x() - 0.5) * size * horizontal + (point.y() - 0.5) * size * up;
    return Ray(origin, direction);
}

float OrthographicCamera::getTMin() const
{
    return std::numeric_limits<float>::lowest();
}

OrthographicCamera::OrthographicCamera(const Vec3f &_center, const Vec3f &_direction, const Vec3f &_up, float _size)
    : center(_center), direction(_direction), up(_up), size(_size)
{
    direction.Normalize();
    rawUp = up;
    rawUp.Normalize();
    Vec3f::Cross3(horizontal, direction, up);
    Vec3f::Cross3(up, horizontal, direction);
    up.Normalize();
    horizontal.Normalize();
}

PerspectiveCamera::PerspectiveCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float _angle_radius) : center(_center), direction(_direction), up(_up), angle_randians(_angle_radius)
{
    direction.Normalize();
    rawUp = up;
    rawUp.Normalize();
    Vec3f::Cross3(horizontal, direction, up);
    Vec3f::Cross3(up, horizontal, direction);
    up.Normalize();
    horizontal.Normalize();
}

Ray PerspectiveCamera::generateRay(Vec2f point)
{
    Vec3f pos = center;
    // Vec3f ray_dir = direction + up * (point.y()-0.5) + horizontal * (point.x()-0.5);
    Vec3f ray_dir = direction + up * (point.y() - 0.5) * tan(angle_randians / 2) * 2 + horizontal * (point.x() - 0.5) * tan(angle_randians / 2) * 2;
    ray_dir.Normalize();
    return Ray(pos, ray_dir);
}

float PerspectiveCamera::getTMin() const
{
    return 0;
}

// ====================================================================
// ====================================================================

void OrthographicCamera::glInit(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-size / 2.0, size / 2.0, -size * (float)h / (float)w / 2.0, size * (float)h / (float)w / 2.0, 0.5, 40.0);
    else
        glOrtho(-size * (float)w / (float)h / 2.0, size * (float)w / (float)h / 2.0, -size / 2.0, size / 2.0, 0.5, 40.0);
}

void OrthographicCamera::glPlaceCamera()
{
    gluLookAt(center.x(), center.y(), center.z(),
              center.x() + direction.x(), center.y() + direction.y(), center.z() + direction.z(),
              up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void OrthographicCamera::dollyCamera(float dist)
{
    center += direction * dist;
}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void OrthographicCamera::truckCamera(float dx, float dy)
{
    center += horizontal * dx + up * dy;
}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void OrthographicCamera::rotateCamera(float rx, float ry)
{
    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(rawUp.Dot3(direction));
    if (tiltAngle - ry > 3.13)
        ry = tiltAngle - 3.13;
    else if (tiltAngle - ry < 0.01)
        ry = tiltAngle - 0.01;

    Matrix rotMat = Matrix::MakeAxisRotation(rawUp, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

    rotMat.Transform(center);
    rotMat.TransformDirection(direction);
}

void PerspectiveCamera::glInit(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angle_randians * 180.0 / 3.14159, (float)w / float(h), 0.5, 40.0);
}

void PerspectiveCamera::glPlaceCamera()
{
    gluLookAt(center.x(), center.y(), center.z(),
              center.x() + direction.x(), center.y() + direction.y(), center.z() + direction.z(),
              up.x(), up.y(), up.z());
}

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void PerspectiveCamera::dollyCamera(float dist)
{
    center += direction * dist;
}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void PerspectiveCamera::truckCamera(float dx, float dy)
{
    center += horizontal * dx + up * dy;
}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void PerspectiveCamera::rotateCamera(float rx, float ry)
{
    // Don't let the model flip upside-down (There is a singularity
    // at the poles when 'up' and 'direction' are aligned)
    float tiltAngle = acos(rawUp.Dot3(direction));
    if (tiltAngle - ry > 3.13)
        ry = tiltAngle - 3.13;
    else if (tiltAngle - ry < 0.01)
        ry = tiltAngle - 0.01;

    Matrix rotMat = Matrix::MakeAxisRotation(rawUp, rx);
    rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

    rotMat.Transform(center);
    rotMat.TransformDirection(direction);
    direction.Normalize();
}
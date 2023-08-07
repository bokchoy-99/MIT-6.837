#ifndef _CAMERA_H
#define _CAMERA_H

#include "vectors.h"
#include "ray.h"

class Camera
{
public:
    Camera() = default;

    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;
};

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera(const Vec3f &_center, const Vec3f &_direction, const Vec3f &_up, float _size);

    Ray generateRay(Vec2f point);
    float getTMin() const;

private:
    Vec3f center, direction, up, horizontal;
    float size;
};

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(const Vec3f &_center, const Vec3f &_direction, const Vec3f &_up, float _angle);

    Ray generateRay(Vec2f point);
    float getTMin() const;

private:
    Vec3f center, direction, up, horizontal;
    float angle;
};

#endif
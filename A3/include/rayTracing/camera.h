#ifndef _CAMERA_H
#define _CAMERA_H

#include <GL/gl.h>
#include <GL/glu.h>
#include "vectors.h"
#include "ray.h"

class Camera
{
public:
    Camera() = default;
    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;

    virtual void glInit(int w, int h) = 0;
    virtual void glPlaceCamera(void) = 0;
    virtual void dollyCamera(float dist) = 0;
    virtual void truckCamera(float dx, float dy) = 0;
    virtual void rotateCamera(float rx, float ry) = 0;

    virtual ~Camera() = default;
};

// 正交相机 -> 正交投影
class OrthographicCamera : public Camera
{
public:
    OrthographicCamera(const Vec3f &_center, const Vec3f &_direction, const Vec3f &_up, float _size);
    ~OrthographicCamera() {}

    Ray generateRay(Vec2f point) override;
    float getTMin() const override;

    void glInit(int w, int h) override;
    void glPlaceCamera() override;
    void dollyCamera(float dist) override;
    void truckCamera(float dx, float dy) override;
    void rotateCamera(float rx, float ry) override;

private:
    Vec3f center, direction, up, horizontal, rawUp;
    float size; // 相机大小
};

// 透视相机 -> 透视投影
class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float angle_randians);
    ~PerspectiveCamera() {}

    Ray generateRay(Vec2f point);
    float getTMin() const;

    void glInit(int w, int h) override;
    void glPlaceCamera() override;
    void dollyCamera(float dist) override;
    void truckCamera(float dx, float dy) override;
    void rotateCamera(float rx, float ry) override;

private:
    Vec3f center, direction, up, horizontal, rawUp;
    float angle_randians;
};

#endif
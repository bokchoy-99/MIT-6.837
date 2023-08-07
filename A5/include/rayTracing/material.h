#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "ray.h"
#include "hit.h"
#include "vectors.h"

// ====================================================================
// ====================================================================

class Material
{

public:
    // CONSTRUCTORS & DESTRUCTOR
    Material() = default;
    Material(const Vec3f &d_color) { diffuseColor = d_color; }
    virtual ~Material() {}

    // ACCESSORS

    virtual Vec3f getDiffuseColor() const { return diffuseColor; }
    virtual Vec3f getSpecularColor() const = 0;
    virtual Vec3f getReflectiveColor() const = 0;
    virtual Vec3f getTransparentColor() const = 0;
    virtual float getIndexOfRefraction() const = 0;

    virtual void glSetMaterial() const = 0;
    virtual Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const = 0;

protected:
    // REPRESENTATION
    Vec3f diffuseColor;
};

// ====================================================================
// ====================================================================

class PhongMaterial : public Material
{
    // 高光颜色、反射的颜色、透明色
    Vec3f specularColor, reflectiveColor, transparentColor;
    // 指数，折射index
    float exponent, indexOfRefraction;

public:
    PhongMaterial() = default;
    PhongMaterial(const Vec3f &_diffuseColor, const Vec3f &specularColor = Vec3f(0, 0, 0), float exponent = 0);
    PhongMaterial(const Vec3f &_diffuseColor, const Vec3f &_specularColor, float _exponent,
                  const Vec3f &_reflectiveColor, const Vec3f &_transparentColor, float _indexOfRefraction);

    Vec3f getSpecularColor() const { return specularColor; }
    Vec3f getReflectiveColor() const { return reflectiveColor; }
    Vec3f getTransparentColor() const { return transparentColor; }
    float getIndexOfRefraction() const { return indexOfRefraction; }

    void glSetMaterial() const override;
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const override;
};

#endif
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

    virtual Vec3f getDiffuseColor(const Vec3f &wsCoord) const { return diffuseColor; }
    // virtual Vec3f getSpecularColor(const Vec3f &wsCoord) const = 0;
    virtual Vec3f getReflectiveColor(const Vec3f &wsCoord) const = 0;
    virtual Vec3f getTransparentColor(const Vec3f &wsCoord) const = 0;
    virtual float getIndexOfRefraction(const Vec3f &wsCoord) const = 0;

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

    // Vec3f getSpecularColor(const Vec3f &wsCoord) const { return specularColor; }
    Vec3f getReflectiveColor(const Vec3f &wsCoord) const { return reflectiveColor; }
    Vec3f getTransparentColor(const Vec3f &wsCoord) const { return transparentColor; }
    float getIndexOfRefraction(const Vec3f &wsCoord) const { return indexOfRefraction; }

    void glSetMaterial() const override;
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const override;
};

// ====================================================================
// ====================================================================

class ProceduralMaterial : public Material
{
    // 变换矩阵：表示从 world space 的坐标转到 texture space
    Matrix *mat;

public:
    explicit ProceduralMaterial(Matrix *m) : mat(m){};
    Vec3f getTextureSpaceCoord(const Vec3f &wsCoord) const;
};

// ====================================================================
// ====================================================================

class CheckerBoard : public ProceduralMaterial
{
    Material *materials[2];
    Material *getMaterial(const Vec3f &wsCoord) const;

public:
    CheckerBoard(Matrix *m, Material *mat1, Material *mat2);
    Vec3f getDiffuseColor(const Vec3f &wsCoord) const override { return getMaterial(wsCoord)->getDiffuseColor(wsCoord); }
    Vec3f getReflectiveColor(const Vec3f &wsCoord) const override { return getMaterial(wsCoord)->getReflectiveColor(wsCoord); }
    Vec3f getTransparentColor(const Vec3f &wsCoord) const override { return getMaterial(wsCoord)->getTransparentColor(wsCoord); }
    float getIndexOfRefraction(const Vec3f &wsCoord) const override { return getMaterial(wsCoord)->getIndexOfRefraction(wsCoord); }

    void glSetMaterial() const { materials[0]->glSetMaterial(); }
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const override;
};

class Noise : public ProceduralMaterial
{
protected:
    Material *m1, *m2;
    int octaves;

    float rangeLength = 0.f, rangeOffset = 0.f;

protected:
    virtual float noise(const Vec3f &tsCoord) const;
    float clampedNoise(const Vec3f &tsCoord) const;
    template <class T>
    T lerp(const T &a, const T &b, float t) const { return a + t * (b - a); }

public:
    Noise(Matrix *m, Material *mat1, Material *mat2, int octaves);

    Vec3f getDiffuseColor(const Vec3f &wsCoord) const override
    {
        auto tsCoord = getTextureSpaceCoord(wsCoord);
        return lerp(m1->getDiffuseColor(wsCoord), m2->getDiffuseColor(wsCoord), clampedNoise(tsCoord));
    }
    Vec3f getReflectiveColor(const Vec3f &wsCoord) const override
    {
        auto tsCoord = getTextureSpaceCoord(wsCoord);
        return lerp(m1->getReflectiveColor(wsCoord), m2->getReflectiveColor(wsCoord), clampedNoise(tsCoord));
    }
    Vec3f getTransparentColor(const Vec3f &wsCoord) const override
    {
        auto tsCoord = getTextureSpaceCoord(wsCoord);
        return lerp(m1->getTransparentColor(wsCoord), m2->getTransparentColor(wsCoord), clampedNoise(tsCoord));
    }
    float getIndexOfRefraction(const Vec3f &wsCoord) const override
    {
        auto tsCoord = getTextureSpaceCoord(wsCoord);
        return lerp(m1->getIndexOfRefraction(wsCoord), m2->getIndexOfRefraction(wsCoord), clampedNoise(tsCoord));
    }

    void glSetMaterial() const override { m1->glSetMaterial(); }
    Vec3f Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const override;
};

class Marble : public Noise
{
    float freq, ampl;

    float noise(const Vec3f &tsCoord) const override;

public:
    Marble(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude);
};

class Wood : public Noise
{
    float freq, ampl;

    float noise(const Vec3f &tsCoord) const override;

public:
    Wood(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude);
};

#endif
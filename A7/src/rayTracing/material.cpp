#include "material.h"
#include "perlin_noise.h"
#include "glCanvas.h"
#include <GL/gl.h>
#include <GL/glut.h>

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

PhongMaterial::PhongMaterial(const Vec3f &diffuseColor,
                             const Vec3f &specularColor, float exponent)
{
    this->diffuseColor = diffuseColor;
    this->specularColor = specularColor;
    this->exponent = exponent;
    reflectiveColor = Vec3f(0, 0, 0);
    transparentColor = Vec3f(0, 0, 0);
    indexOfRefraction = 1; // 默认空气折射率1
}

PhongMaterial::PhongMaterial(const Vec3f &_diffuseColor, const Vec3f &_specularColor, float _exponent,
                             const Vec3f &_reflectiveColor, const Vec3f &_transparentColor, float _indexOfRefraction)
    : Material(_diffuseColor)
{
    specularColor = _specularColor;
    exponent = _exponent;
    reflectiveColor = _reflectiveColor;
    transparentColor = _transparentColor;
    indexOfRefraction = _indexOfRefraction;
}

void PhongMaterial::glSetMaterial() const
{
    GLfloat one[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat zero[4] = {0.0, 0.0, 0.0, 0.0};
    GLfloat specular[4] = {
        specularColor.r(),
        specularColor.g(),
        specularColor.b(),
        1.0};
    GLfloat diffuse[4] = {
        diffuseColor.r(),
        diffuseColor.g(),
        diffuseColor.b(),
        1.0};

    // NOTE: GL uses the Blinn Torrance version of Phong...
    float glExponent = exponent;
    if (glExponent < 0)
        glExponent = 0;
    if (glExponent > 128)
        glExponent = 128;

#if !SPECULAR_FIX

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glExponent);

#else
    // OPTIONAL: 3 pass rendering to fix the specular highlight
    // artifact for small specular exponents (wide specular lobe)

    if (SPECULAR_FIX_WHICH_PASS == 0)
    {
        // First pass, draw only the specular highlights
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glExponent);
    }
    else if (SPECULAR_FIX_WHICH_PASS == 1)
    {
        // Second pass, compute normal dot light
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }
    else
    {
        // Third pass, add ambient & diffuse terms
        assert(SPECULAR_FIX_WHICH_PASS == 2);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }
#endif
}

// 根据光线与交点情况 着色
Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
    Vec3f v = ray.getDirection();
    v.Negate();
    Vec3f h = v + dirToLight;
    h.Normalize();

    auto diffuse = std::max(0.f, dirToLight.Dot3(hit.getNormal())) * lightColor * diffuseColor;
    // ignore r^2 coefficient
    auto specular = specularColor * lightColor * powf(hit.getNormal().Dot3(h), exponent);
    return diffuse + specular;
}

// ====================================================================
// ====================================================================

Vec3f ProceduralMaterial::getTextureSpaceCoord(const Vec3f &wsCoord) const
{
    auto tsCoord = wsCoord;
    mat->Transform(tsCoord);
    return tsCoord;
}

// ====================================================================
// ====================================================================

Material *CheckerBoard::getMaterial(const Vec3f &wsCoord) const
{
    auto tsp = getTextureSpaceCoord(wsCoord);
    // 一个 1 × 1 × 1 的方格内是同一材料
    auto index = int(fabsf(floorf(tsp.x()) + floorf(tsp.y()) + floorf(tsp.z()))) % 2;
    return materials[index];
}

CheckerBoard::CheckerBoard(Matrix *m, Material *mat1, Material *mat2) : ProceduralMaterial(m)
{
    materials[0] = mat1;
    materials[1] = mat2;
}

Vec3f CheckerBoard::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
    const auto &intersection = hit.getIntersectionPoint();
    return getMaterial(intersection)->Shade(ray, hit, dirToLight, lightColor);
}

Noise::Noise(Matrix *m, Material *mat1, Material *mat2, int _octaves) : ProceduralMaterial(m)
{
    m1 = mat1;
    m2 = mat2;
    octaves = _octaves;
    rangeLength = 1.f;
    rangeOffset = 0.5f;
}

float Noise::noise(const Vec3f &tsCoord) const
{
    float noise = 0.0f;
    auto noiseP = tsCoord;
    float noiseScale = 1.f;
    for (int i = 0; i < octaves; ++i)
    {
        float x, y, z;
        noiseP.Get(x, y, z);
        noise += PerlinNoise::noise(x, y, z) / noiseScale;
        noiseP *= 2.f;
        noiseScale *= 2.f;
    }
    return noise;
}

float Noise::clampedNoise(const Vec3f &tsCoord) const
{
    float n = (rangeOffset - noise(tsCoord)) / rangeLength;
    return std::min(1.f, std::max(0.f, n));
}

Vec3f Noise::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight, const Vec3f &lightColor) const
{
    auto color1 = m1->Shade(ray, hit, dirToLight, lightColor);
    auto color2 = m2->Shade(ray, hit, dirToLight, lightColor);
    float noise = clampedNoise(getTextureSpaceCoord(hit.getIntersectionPoint()));
    return lerp(color1, color2, noise);
}

Marble::Marble(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) : Noise(m, mat1, mat2, octaves)
{
    freq = frequency;
    ampl = amplitude;
    rangeLength = 2.f;
    rangeOffset = 1.f;
}

float Marble::noise(const Vec3f &tsCoord) const
{
    return sinf(freq * tsCoord.x() + ampl * Noise::noise(tsCoord));
}

Wood::Wood(Matrix *m, Material *mat1, Material *mat2, int octaves, float frequency, float amplitude) : Noise(m, mat1, mat2, octaves)
{
    freq = frequency;
    ampl = amplitude;
    rangeLength = 2.f;
    rangeOffset = 1.f;
}

float Wood::noise(const Vec3f &tsCoord) const
{
    float l = tsCoord.x() * tsCoord.x() + tsCoord.z() * tsCoord.z();
    float s = sinf(freq * sqrtf(l) + ampl * Noise::noise(tsCoord) + tsCoord.y() * 0.4f);
    float sign = s < 0 ? -1.f : 1.f;
    s = fabsf(s);
    return sign * (1 - powf(1 - s, freq));
}

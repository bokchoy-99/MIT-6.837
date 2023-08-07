
#include "raytracer.h"

#include <cmath>
#include <vector>
#include "light.h"
#include "rayTree.h"

static float eps = 1e-5f;

RayTracer::RayTracer(const std::shared_ptr<SceneParser> &_sceneParser, const shared_ptr<CommandLineArgumentParser> &_cmdlParser)
{
    sceneParser = _sceneParser;
    cmdlParser = _cmdlParser;
    // 场景信息
    bgColor = sceneParser->getBackgroundColor();
    group = sceneParser->getGroup();
    numLights = sceneParser->getNumLights();
    lights = vector<Light *>(numLights);
    for (int i = 0; i < numLights; i++)
    {
        lights[i] = sceneParser->getLight(i);
    }
    ambient = sceneParser->getAmbientLight();
}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const
{
    Vec3f color(0.f, 0.f, 0.f);

    if (bounces > cmdlParser->max_bounces || weight <= cmdlParser->cutoff_weight)
        return color;

    if (group->intersect(ray, hit, tmin))
    {
        // 处理相交点的颜色
        // 处理是否对背面的情况着色
        auto norm = hit.getNormal();
        bool isBackSide = norm.Dot3(ray.getDirection()) > 0;
        if (isBackSide)
        {
            if (cmdlParser->shade_back)
            {
                norm.Negate();
                hit.set(hit.getT(), hit.getMaterial(), norm, ray);
            }
            else
            {
                return color;
            }
        }

        // add help line
        if (bounces == 0)
            RayTree::SetMainSegment(ray, 0, hit.getT());

        auto material = hit.getMaterial();
        Vec3f intersection = hit.getIntersectionPoint();
        color = ambient * material->getDiffuseColor();

        for (int i = 0; i < numLights; i++)
        {
            float distanceToLight;
            Vec3f dirToLight, lightColor;
            lights[i]->getIllumination(intersection, dirToLight, lightColor, distanceToLight);

            // 处理阴影
            if (cmdlParser->cast_shadow)
            {
                Ray shadowRay(intersection, dirToLight);
                Hit shadowHit;
                // 如果在从交点射向光源的射线，与场景中其他的物体（交点与光源之间）相交，舍弃该光源的贡献
                if (group->intersect(shadowRay, shadowHit, eps * 100.f) && shadowHit.getT() < distanceToLight)
                {
                    RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
                    continue;
                }
                else
                {
                    RayTree::AddShadowSegment(shadowRay, 0, distanceToLight);
                }
            }
            color += material->Shade(ray, hit, dirToLight, lightColor);
        }

        // 处理反射光线
        auto reflectColor = material->getReflectiveColor();
        if (reflectColor.Length() > eps)
        {
            auto reflectDir = mirrorDirection(hit.getNormal(), ray.getDirection());
            Ray reflectRay(intersection, reflectDir);
            Hit reflectHit;
            color += reflectColor * traceRay(reflectRay, eps, bounces + 1, weight * reflectColor.Length() / sqrtf(3),
                                             indexOfRefraction, reflectHit);
            if (bounces < cmdlParser->max_bounces)
                RayTree::AddReflectedSegment(reflectRay, 0, reflectHit.getT());
        }

        // 处理折射光线
        auto transparentColor = material->getTransparentColor();
        // 如果是 entering transmissive material，即 ray * norm < 0, 取材料的折射率
        // 如果是 leaving transmissive material，即 ray * norm > 0, 折射率取 1.f, 因为当前在材料内
        auto materialIndexOfRefraction = isBackSide ? 1.f : material->getIndexOfRefraction();
        if (transparentColor.Length() > eps)
        {
            Vec3f transmittedDir;
            auto transmitted = transmittedDirection(hit.getNormal(), ray.getDirection(),
                                                    indexOfRefraction, materialIndexOfRefraction, transmittedDir);
            if (transmitted)
            {
                Ray refractRay(intersection, transmittedDir);
                Hit refractHit;
                color += transparentColor * traceRay(refractRay, eps, bounces + 1, weight * reflectColor.Length() / sqrtf(3),
                                                     materialIndexOfRefraction, refractHit);
                if (bounces < cmdlParser->max_bounces)
                {
                    RayTree::AddTransmittedSegment(refractRay, 0, refractHit.getT());
                }
            }
        }
    }
    else
    {
        // add help line
        RayTree::SetMainSegment(ray, 0, INFINITY);
        color = bgColor;
    }
    return color;
}

// 得到反射光线的方向
Vec3f RayTracer::mirrorDirection(const Vec3f &normal, const Vec3f &incoming) const
{
    // 得到反射光线的方向向量
    auto reflectDir = incoming - 2.f * (incoming.Dot3(normal)) * normal;
    reflectDir.Normalize();
    return reflectDir;
}

// 得到折射光线的方向，从 介质i 进入 介质t
bool RayTracer::transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
                                     float index_i, float index_t, Vec3f &transmitted) const
{
    // [ηr * (N · I) – √(1 – ηr^2 * (1 – (N · I)^2))] * N – ηr * I
    auto i = incoming;
    i.Negate();
    auto eta = index_i / index_t;
    auto cosThetaTSquare = 1.f - eta * eta * (1.f - normal.Dot3(i) * normal.Dot3(i));
    if (cosThetaTSquare < 0.f)
        return false;
    transmitted = (eta * normal.Dot3(i) - sqrt(cosThetaTSquare)) * normal - eta * i;
    transmitted.Normalize();
    return true;
}

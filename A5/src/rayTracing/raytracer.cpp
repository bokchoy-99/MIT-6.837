
#include "raytracer.h"

#include <cmath>
#include <vector>
#include "light.h"
#include "rayTree.h"

static float eps = 1e-3f;

RayTracer::RayTracer(const std::shared_ptr<SceneParser> &_sceneParser, const shared_ptr<CommandLineArgumentParser> &_cmdlParser)
{
    sceneParser = _sceneParser;
    cmdlParser = _cmdlParser;
    // 获取场景信息
    bgColor = sceneParser->getBackgroundColor();
    group = sceneParser->getGroup();
    numLights = sceneParser->getNumLights();
    lights = vector<Light *>(numLights);
    for (int i = 0; i < numLights; i++)
    {
        lights[i] = sceneParser->getLight(i);
    }
    ambient = sceneParser->getAmbientLight();

    if (cmdlParser->grid_nx > 0 && cmdlParser->grid_ny > 0 && cmdlParser->grid_nz > 0)
    {
        grid = new Grid(group->getBoundingBox(), cmdlParser->grid_nx, cmdlParser->grid_ny, cmdlParser->grid_nz);
        group->insertIntoGrid(grid, nullptr);
    }
}

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const
{
    // 达到递归深度，直接返回
    if (bounces > cmdlParser->max_bounces)
    {
        return Vec3f(0.f, 0.f, 0.f);
    }
    // 权重贡献小于阈值，直接返回
    if (weight < cmdlParser->cutoff_weight)
    {
        return Vec3f(0.f, 0.f, 0.f);
    }

    Vec3f color;
    Object3D *scene;
    if (cmdlParser->visualize_grid)
    {
        scene = grid;
    }
    else
    {
        scene = group;
    }
    if (scene->intersect(ray, hit, tmin))
    {
        auto norm = hit.getNormal();
        bool isBackSide = norm.Dot3(ray.getDirection()) > 0;
        if (isBackSide)
        {
            if (cmdlParser->shade_back)
            {
                norm.Negate();
                hit.set(hit.getT(), hit.getMaterial(), norm, ray, hit.getObjectType());
            }
            else
            {
                return Vec3f(0.f, 0.f, 0.f);
            }
        }

        // add help line
        float mainT = hit.getT();
        if (cmdlParser->visualize_grid)
            mainT = INFINITY;
        if (bounces == 0)
            RayTree::SetMainSegment(ray, 0, mainT);

        auto material = hit.getMaterial();
        // 物体在环境光中的颜色
        color = ambient * material->getDiffuseColor();
        // 找到交点
        auto intersection = hit.getIntersectionPoint();
        for (int i = 0; i < numLights; i++)
        {
            Vec3f dirToLight, lightColor;
            float distToLight;
            lights[i]->getIllumination(intersection, dirToLight, lightColor, distToLight);

            // 处理阴影部分，首先从焦点出发，对每个光源生成射线，
            // 如果射线在光源前与物体相交，舍弃该光源对该像素的颜色贡献。
            if (cmdlParser->cast_shadow)
            {
                Ray shadowRay(intersection, dirToLight);
                Hit shadowHit;
                // 如果在从交点射向光源的射线，与场景中其他的物体（交点与光源之间）相交，舍弃该光源的贡献
                if (scene->intersect(shadowRay, shadowHit, eps) && shadowHit.getT() < distToLight)
                {
                    RayTree::AddShadowSegment(shadowRay, 0, shadowHit.getT());
                    continue;
                    // 在这里直接跳出，舍弃该光源对该像素的颜色贡献。
                }
                else
                {
                    RayTree::AddShadowSegment(shadowRay, 0, distToLight);
                };
            }
            color += material->Shade(ray, hit, dirToLight, lightColor);
        }

        // 处理反射光，进行递归
        auto reflectColor = material->getReflectiveColor();
        if (reflectColor.Length() > eps)
        {
            Hit reflectHit;
            Ray reflectRay(intersection, mirrorDirection(hit.getNormal(), ray.getDirection()));
            color += reflectColor * traceRay(reflectRay, eps, bounces + 1, weight * reflectColor.Length() / sqrtf(3),
                                             indexOfRefraction, reflectHit);
            if (bounces < cmdlParser->max_bounces)
                RayTree::AddReflectedSegment(reflectRay, 0, reflectHit.getT());
        }

        // // 处理折射光，进行递归
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
                Hit refractHit;
                Ray refractRay(intersection, transmittedDir);
                color += transparentColor *
                         traceRay(refractRay, eps, bounces + 1, weight * transparentColor.Length() / sqrtf(3),
                                  materialIndexOfRefraction, refractHit);
                if (bounces < cmdlParser->max_bounces)
                    RayTree::AddTransmittedSegment(refractRay, 0, refractHit.getT());
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
    auto reflectDir = incoming - 2 * (incoming.Dot3(normal)) * normal;
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
    transmitted = (eta * (normal.Dot3(i)) - sqrtf(cosThetaTSquare)) * normal - eta * i;
    transmitted.Normalize();
    return true;
}

#include "raytracer.h"
#include <vector>
#include <cmath>
#include "light.h"

void RayTracer::readSceneFromFile(char *input_file)
{
    // 读取参数文件
    sceneParser = std::make_shared<SceneParser>(input_file);
}

void RayTracer::renderToImage(Image &img, bool shade_back)
{
    auto blackColor = Vec3f(0.f, 0.f, 0.f);
    auto bgcolor = sceneParser->getBackgroundColor();
    img.SetAllPixels(bgcolor);

    auto camera = sceneParser->getCamera();
    assert(camera != nullptr);

    auto group = sceneParser->getGroup();

    auto nLights = sceneParser->getNumLights();
    vector<Light *> lights(nLights);
    for (int i = 0; i < nLights; i++)
    {
        lights[i] = sceneParser->getLight(i);
    }
    auto ambient = sceneParser->getAmbientLight();

    for (int x = 0; x < img.Width(); x++)
    {
        for (int y = 0; y < img.Height(); y++)
        {
            Vec2f point = Vec2f(float(x) / img.Width(), float(y) / img.Height());
            Ray ray = camera->generateRay(point);
            Hit hit;
            if (group->intersect(ray, hit, camera->getTMin()))
            {
                auto norm = hit.getNormal();
                bool isBackSide = norm.Dot3(ray.getDirection()) > 0;
                if (isBackSide)
                {
                    if (shade_back) {
                        norm.Negate();
                    }
                    else {
                        img.SetPixel(x, y, blackColor);
                        continue;
                    }
                }

                auto m = hit.getMaterial();
                auto cObj = m->getDiffuseColor();
                Vec3f color = ambient * cObj;

                for (int i = 0; i < nLights; ++i)
                {
                    Vec3f intersection = hit.getIntersectionPoint(), lightDir, cLight;
                    lights[i]->getIllumination(intersection, lightDir, cLight);
                    color += std::max(0.f, lightDir.Dot3(norm)) * cLight * cObj;
                }
                img.SetPixel(x, y, color);
            }
        }
    }
}

void RayTracer::renderDepthToImage(Image &img, float depMin, float depMax)
{
    auto blackcColor = Vec3f(0.f, 0.f, 0.f), whiteColor = Vec3f(1.f, 1.f, 1.f);
    img.SetAllPixels(blackcColor);

    auto camera = sceneParser->getCamera();
    assert(camera != nullptr);

    auto group = sceneParser->getGroup();

    for (int x = 0; x < img.Width(); x++)
    {
        for (int y = 0; y < img.Height(); y++)
        {
            Vec2f point = Vec2f(float(x) / img.Width(), float(y) / img.Height());
            Ray ray = camera->generateRay(point);
            Hit h;
            if (group->intersect(ray, h, camera->getTMin()))
            {
                auto t = h.getT();
                t = std::min(t, depMax);
                t = std::max(t, depMin);
                float weight = (t - depMin) / (depMax - depMin);
                Vec3f color;
                Vec3f::WeightedSum(color, blackcColor, weight, whiteColor, 1 - weight);
                img.SetPixel(x, y, color);
            }
        }
    }
}

void RayTracer::renderNormalToImage(Image &img)
{
    auto blackColor = Vec3f(0.f, 0.f, 0.f);
    img.SetAllPixels(blackColor);

    auto camera = sceneParser->getCamera();
    assert(camera != nullptr);

    auto group = sceneParser->getGroup();

    for (int y = 0; y < img.Width(); ++y)
    {
        for (int x = 0; x < img.Height(); ++x)
        {
            Vec2f p(float(x) / img.Width(), float(y) / img.Height());
            auto ray = camera->generateRay(p);
            Hit hit;
            if (group->intersect(ray, hit, camera->getTMin()))
            {
                auto norm = hit.getNormal();
                Vec3f color = Vec3f(fabsf(norm.x()), fabsf(norm.y()), fabsf(norm.z()));
                img.SetPixel(x, y, color);
            }
        }
    }
}

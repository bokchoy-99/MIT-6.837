
#include "raytracer.h"

#include <cmath>

void RayTracer::readSceneFromFile(char *input_file)
{
    // 读取参数文件
    sceneParser = std::make_shared<SceneParser>(input_file);
}

void RayTracer::renderToImage(Image &img)
{
    auto bgcolor = sceneParser->getBackgroundColor();
    img.SetAllPixels(bgcolor);

    auto camera = dynamic_cast<OrthographicCamera *>(sceneParser->getCamera());
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
                Material *m = h.getMaterial();
                Vec3f color = m->getDiffuseColor();
                img.SetPixel(x, y, color);
            }
        }
    }
}

void RayTracer::renderDepthToImage(Image &img, float depMin, float depMax)
{
    auto blackcColor = Vec3f(0.f, 0.f, 0.f), whiteColor = Vec3f(1.f, 1.f, 1.f);
    img.SetAllPixels(blackcColor);

    auto camera = dynamic_cast<OrthographicCamera *>(sceneParser->getCamera());
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
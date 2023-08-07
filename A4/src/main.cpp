#include <memory>
#include <cmath>

#include "commandLineArgumentParser.h"
#include "image.h"
#include "raytracer.h"
#include "scene_parser.h"
#include "glCanvas.h"
#include "camera.h"
#include "rayTree.h"

std::shared_ptr<CommandLineArgumentParser> CmdlParser;
std::shared_ptr<SceneParser> ScParser;
std::shared_ptr<RayTracer> RTracer;

void renderFunc()
{
    auto imgColor = Image(CmdlParser->width, CmdlParser->height);
    auto imgNormal = Image(CmdlParser->width, CmdlParser->height);
    auto imgDepth = Image(CmdlParser->width, CmdlParser->height);

    imgColor.SetAllPixels(Vec3f(0.f, 0.f, 0.f));
    imgNormal.SetAllPixels(Vec3f(0.f, 0.f, 0.f));
    imgDepth.SetAllPixels(Vec3f(0.f, 0.f, 0.f));

    auto *camera = ScParser->getCamera();
    assert(camera != nullptr);

    for (int x = 0; x < CmdlParser->width; x++)
    {
        for (int y = 0; y < CmdlParser->height; y++)
        {
            Vec2f point(float(x) / CmdlParser->width, float(y) / CmdlParser->height);
            Ray ray = camera->generateRay(point);
            Hit hit;
            Vec3f color = RTracer->traceRay(ray, camera->getTMin(), 0.f, 1.f, 1.f, hit);

            if (CmdlParser->output_file != nullptr)
            {
                imgColor.SetPixel(x, y, color);
            }

            if (CmdlParser->depth_file != nullptr)
            {
                auto t = hit.getT();
                t = std::min(CmdlParser->depth_max, t);
                t = std::max(CmdlParser->depth_min, t);
                float weight = (t - CmdlParser->depth_min) / (CmdlParser->depth_max - CmdlParser->depth_min);
                Vec3f depthColor;
                Vec3f::WeightedSum(depthColor, Vec3f(0.f, 0.f, 0.f), weight, Vec3f(1.f, 1.f, 1.f), 1 - weight);
                imgDepth.SetPixel(x, y, depthColor);
            }

            if (CmdlParser->normal_file != nullptr)
            {
                auto norm = hit.getNormal();
                Vec3f normalColor = Vec3f(fabsf(norm.x()), fabsf(norm.y()), fabsf(norm.z()));
                imgNormal.SetPixel(x, y, normalColor);
            }
        }
    }

    if (CmdlParser->output_file != nullptr)
    {
        imgColor.SaveTGA(CmdlParser->output_file);
    }

    if (CmdlParser->depth_file != nullptr)
    {
        imgDepth.SaveTGA(CmdlParser->depth_file);
    }

    if (CmdlParser->normal_file != nullptr)
    {
        imgNormal.SaveTGA(CmdlParser->normal_file);
    }
}

void traceRayFunc(float x, float y)
{
    auto camera = ScParser->getCamera();
    assert(camera != nullptr);
    Vec2f p(x, y);
    auto ray = camera->generateRay(p);
    Hit hit;
    RTracer->traceRay(ray, camera->getTMin(), 0, 1.f, 1.f, hit);
}

int main(int argc, char *argv[])
{
    CmdlParser = std::make_shared<CommandLineArgumentParser>();
    CmdlParser->parse(argc, argv);
    ScParser = std::make_shared<SceneParser>(CmdlParser->input_file);

    RTracer = std::make_shared<RayTracer>(ScParser, CmdlParser);

    if (CmdlParser->use_gui)
    {
        GLCanvas canvas;
        canvas.initialize(ScParser.get(), renderFunc, traceRayFunc);
    }
    else
    {
        renderFunc();
    }
}

// main -input ../assets/scene4_01_sphere_shadow.txt -size 200 200 -output output4_01.tga -shadows

// main -input ../assets/scene4_02_colored_shadows.txt -size 200 200 -output output4_02.tga -shadows -gui -tessellation 50 25 -gouraud

// main -input ../assets/scene4_03_mirrored_floor.txt -size 200 200 -output output4_03.tga -shadows -bounces 1 -weight 0.01 -gui

// main -input ../assets/scene4_04_reflective_sphere.txt -size 200 200 -output output4_04a.tga -shadows -bounces 0 -weight 0.01
// main -input ../assets/scene4_04_reflective_sphere.txt -size 200 200 -output output4_04b.tga -shadows -bounces 1 -weight 0.01
// main -input ../assets/scene4_04_reflective_sphere.txt -size 200 200 -output output4_04c.tga -shadows -bounces 2 -weight 0.01
// main -input ../assets/scene4_04_reflective_sphere.txt -size 200 200 -output output4_04d.tga -shadows -bounces 3 -weight 0.01

// main -input ../assets/scene4_05_transparent_bar.txt -size 200 200 -output output4_05.tga -shadows -bounces 10 -weight 0.01 -shade_back -gui

// main -input ../assets/scene4_06_transparent_bars.txt -size 200 200 -output output4_06a.tga -shadows -bounces 0 -weight 0.01 -shade_back -gui
// main -input ../assets/scene4_06_transparent_bars.txt -size 200 200 -output output4_06b.tga -shadows -bounces 1 -weight 0.01 -shade_back -gui
// main -input ../assets/scene4_06_transparent_bars.txt -size 200 200 -output output4_06c.tga -shadows -bounces 2 -weight 0.01 -shade_back -gui
// main -input ../assets/scene4_06_transparent_bars.txt -size 200 200 -output output4_06d.tga -shadows -bounces 3 -weight 0.01 -shade_back -gui
// main -input ../assets/scene4_06_transparent_bars.txt -size 200 200 -output output4_06e.tga -shadows -bounces 4 -weight 0.01 -shade_back -gui
// main -input ../assets/scene4_06_transparent_bars.txt -size 200 200 -output output4_06f.tga -shadows -bounces 5 -weight 0.01 -shade_back -gui

// main -input ../assets/scene4_07_transparent_sphere_1.0.txt -size 200 200 -output output4_07.tga -shadows -bounces 5 -weight 0.01 -shade_back -tessellation 30 15 -gui
// main -input ../assets/scene4_08_transparent_sphere_1.1.txt -size 200 200 -output output4_08.tga -shadows -bounces 5 -weight 0.01 -shade_back -tessellation 30 15 -gui
// main -input ../assets/scene4_09_transparent_sphere_2.0.txt -size 200 200 -output output4_09.tga -shadows -bounces 5 -weight 0.01 -shade_back -tessellation 30 15 -gui

// main -input ../assets/scene4_10_point_light_distance.txt -size 200 200 -output output4_10.tga -shadows -gui

// main -input ../assets/scene4_11_point_light_circle.txt -size 200 200 -output output4_11.tga -shadows
// main -input ../assets/scene4_12_point_light_circle_d_attenuation.txt -size 200 200 -output output4_12.tga -shadows
// main -input ../assets/scene4_13_point_light_circle_d2_attenuation.txt -size 200 200 -output output4_13.tga -shadows

// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -output output4_14a.tga -shadows -shade_back -bounces 0 -weight 0.01
// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -output output4_14b.tga -shadows -shade_back -bounces 1 -weight 0.01
// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -output output4_14c.tga -shadows -shade_back -bounces 2 -weight 0.01
// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -output output4_14d.tga -shadows -shade_back -bounces 3 -weight 0.01
// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -output output4_14e.tga -shadows -shade_back -bounces 4 -weight 0.01
// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -output output4_14f.tga -shadows -shade_back -bounces 5 -weight 0.01

// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -shadows -shade_back -bounces 1 -weight 0.01 -gui
// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -shadows -shade_back -bounces 2 -weight 0.01 -gui
// main -input ../assets/scene4_14_faceted_gem.txt -size 200 200 -shadows -shade_back -bounces 3 -weight 0.01 -gui

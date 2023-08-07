//
// Created by Nycshisan on 2018/7/23.
//

#include <memory>
#include <cmath>

#include "commandLineArgumentParser.h"
#include "image.h"
#include "raytracer.h"
#include "scene_parser.h"
#include "glCanvas.h"
#include "camera.h"
#include "rayTree.h"
#include "grid.h"
#include "raytracing_stats.h"

std::shared_ptr<CommandLineArgumentParser> CmdlParser;
std::shared_ptr<SceneParser> ScParser;
std::shared_ptr<RayTracer> RTracer;

void traceRayFunc(float x, float y)
{

    RayTracingStats::Initialize(CmdlParser->width, CmdlParser->height, RTracer->getBoundingBox(),
                                CmdlParser->grid_nx, CmdlParser->grid_ny, CmdlParser->grid_nz);

    auto *camera = ScParser->getCamera();
    assert(camera != nullptr);
    Vec2f p(x, y);
    auto ray = camera->generateRay(p);
    Hit hit;
    RTracer->getGrid()->refreshColorSchema();
    RTracer->traceRay(ray, camera->getTMin(), 0, 1.f, 1.f, hit);

    if (CmdlParser->stats)
        RayTracingStats::PrintStatistics();
}

void renderFunc()
{
    RayTracingStats::Initialize(CmdlParser->width, CmdlParser->height, RTracer->getBoundingBox(),
                                CmdlParser->grid_nx, CmdlParser->grid_ny, CmdlParser->grid_nz);
    auto imgColor = Image(CmdlParser->width, CmdlParser->height);
    auto imgNormal = Image(CmdlParser->width, CmdlParser->height);
    auto imgDepth = Image(CmdlParser->width, CmdlParser->height);

    imgColor.SetAllPixels(Vec3f(0.f, 0.f, 0.f));
    imgNormal.SetAllPixels(Vec3f(0.f, 0.f, 0.f));
    imgDepth.SetAllPixels(Vec3f(0.f, 0.f, 0.f));

    auto *camera = ScParser->getCamera();
    assert(camera != nullptr);

    for (int x = 0; x < CmdlParser->width; ++x)
    {
        for (int y = 0; y < CmdlParser->height; ++y)
        {
            Vec2f p(float(x) / CmdlParser->width, float(y) / CmdlParser->height);
            auto ray = camera->generateRay(p);
            Hit hit;
            auto color = RTracer->traceRay(ray, camera->getTMin(), 0, 1.f, 1.f, hit);

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

    if (CmdlParser->stats)
        RayTracingStats::PrintStatistics();
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
        canvas.initialize(ScParser.get(), renderFunc, traceRayFunc, RTracer->getGrid(), CmdlParser->visualize_grid);
    }
    else
    {
        renderFunc();
    }
}

// main -input ../assets/scene6_01_sphere.txt -output output6_01a.tga -size 200 200 -stats
// main -input ../assets/scene6_01_sphere.txt -output output6_01b.tga -size 200 200 -grid 10 10 10 -stats
// main -input ../assets/scene6_01_sphere.txt -output output6_01c.tga -size 200 200 -grid 10 10 10 -visualize_grid

// main -input ../assets/scene6_02_sphere_triangles.txt -output output6_02a.tga -size 200 200 -stats
// main -input ../assets/scene6_02_sphere_triangles.txt -output output6_02b.tga -size 200 200 -grid 10 10 10 -stats
// main -input ../assets/scene6_02_sphere_triangles.txt -output output6_02c.tga -size 200 200 -stats -shadows
// main -input ../assets/scene6_02_sphere_triangles.txt -output output6_02d.tga -size 200 200 -grid 10 10 10 -stats -shadows
// main -input ../assets/scene6_02_sphere_triangles.txt -output output6_02e.tga -size 200 200 -grid 10 10 10 -visualize_grid

// main -input ../assets/scene6_03_sphere_plane.txt -output output6_03a.tga -size 200 200 -stats
// main -input ../assets/scene6_03_sphere_plane.txt -output output6_03b.tga -size 200 200 -grid 10 10 10 -stats
// main -input ../assets/scene6_03_sphere_plane.txt -output output6_03c.tga -size 200 200 -stats -shadows
// main -input ../assets/scene6_03_sphere_plane.txt -output output6_03d.tga -size 200 200 -grid 10 10 10 -stats -shadows
// main -input ../assets/scene6_03_sphere_plane.txt -output output6_03e.tga -size 200 200 -grid 10 10 10 -visualize_grid

// main -input ../assets/scene6_04_bunny_mesh_200.txt -output output6_04a.tga -size 200 200 -stats
// main -input ../assets/scene6_06_bunny_mesh_5k.txt  -output output6_045.tga -size 200 200 -stats

// main -input ../assets/scene6_04_bunny_mesh_200.txt -output output6_04b.tga -size 200 200 -grid 10 10 7 -stats
// main -input ../assets/scene6_04_bunny_mesh_200.txt -output output6_04c.tga -size 200 200 -stats -shadows
// main -input ../assets/scene6_04_bunny_mesh_200.txt -output output6_04d.tga -size 200 200 -grid 10 10 7 -stats -shadows
// main -input ../assets/scene6_04_bunny_mesh_200.txt -output output6_04e.tga -size 200 200 -grid 10 10 7 -visualize_grid
// main -input ../assets/scene6_05_bunny_mesh_1k.txt  -output output6_05.tga -size 200 200 -grid 15 15 12 -stats -shadows
// main -input ../assets/scene6_06_bunny_mesh_5k.txt  -output output6_06.tga -size 200 200 -grid 20 20 15 -stats -shadows
// main -input ../assets/scene6_07_bunny_mesh_40k.txt -output output6_07.tga -size 200 200 -grid 40 40 33 -stats -shadows
// main -input ../assets/scene6_07_bunny_mesh_40k.txt -output output6_07a.tga -size 200 200 -grid 40 40 33 -visualize_grid

// main -input ../assets/scene6_08_scale_translate.txt -size 200 200 -output output6_08a.tga
// main -input ../assets/scene6_08_scale_translate.txt -size 200 200 -output output6_08b.tga -grid 15 15 15
// main -input ../assets/scene6_08_scale_translate.txt -size 200 200 -output output6_08c.tga -grid 15 15 15 -visualize_grid

// main -input ../assets/scene6_09_rotated_triangles.txt -size 200 200 -output output6_09a.tga
// main -input ../assets/scene6_09_rotated_triangles.txt -size 200 200 -output output6_09b.tga -grid 15 15 9
// main -input ../assets/scene6_09_rotated_triangles.txt -size 200 200 -output output6_09c.tga -grid 15 15 9 -visualize_grid

// main -input ../assets/scene6_10_nested_transformations.txt -size 200 200 -output output6_10a.tga
// main -input ../assets/scene6_10_nested_transformations.txt -size 200 200 -output output6_10b.tga -grid 30 30 30
// main -input ../assets/scene6_10_nested_transformations.txt -size 200 200 -output output6_10c.tga -grid 30 30 30 -visualize_grid

// main -input ../assets/scene6_11_mirrored_floor.txt -size 200 200 -output output6_11a.tga -shadows -bounces 1 -weight 0.01 -stats
// main -input ../assets/scene6_11_mirrored_floor.txt -size 200 200 -output output6_11b.tga -shadows -bounces 1 -weight 0.01 -grid 40 10 40 -stats
// main -input ../assets/scene6_11_mirrored_floor.txt -size 200 200 -output output6_11c.tga -grid 40 10 40 -visualize_grid

// main -input ../assets/scene6_12_faceted_gem.txt -size 200 200 -output output6_12a.tga -shadows -shade_back -bounces 5 -weight 0.01 -stats
// main -input ../assets/scene6_12_faceted_gem.txt -size 200 200 -output output6_12b.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20 -stats
// main -input ../assets/scene6_12_faceted_gem.txt -size 200 200 -output output6_12c.tga -grid 20 20 20 -visualize_grid


// 仔细看下
// Texture
// main -input ../assets/scene6_13_checkerboard.txt -size 200 200 -output output6_13.tga -shadows
// main -input ../assets/scene6_14_glass_sphere.txt -size 200 200 -output output6_14.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20

// main -input ../assets/scene6_15_marble_cubes.txt -size 300 300 -output output6_15.tga 
// main -input ../assets/scene6_16_wood_cubes.txt -size 300 300 -output output6_16.tga 

// main -input ../assets/scene6_17_marble_vase.txt -size 300 300 -output output6_17a.tga -grid 15 30 15 -bounces 1 -shadows
// main -input ../assets/scene6_17_marble_vase.txt -size 300 300 -output output6_17b.tga -grid 15 30 15 -visualize_grid

// main -input ../assets/scene6_18_6.837_logo.txt -size 400 200 -output output6_18a.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 80 30 3
// main -input ../assets/scene6_18_6.837_logo.txt -size 400 200 -output output6_18b.tga -grid 80 30 3 -visualize_grid

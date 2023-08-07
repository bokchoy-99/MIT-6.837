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

std::shared_ptr<CommandLineArgumentParser> CmdlParser;
std::shared_ptr<SceneParser> ScParser;
std::shared_ptr<RayTracer> RTracer;

void traceRayFunc(float x, float y) {
    auto *camera = ScParser->getCamera();
    assert(camera != nullptr);
    Vec2f p(x, y);
    auto ray = camera->generateRay(p);
    Hit hit;
    RTracer->getGrid()->refreshColorSchema();
    RTracer->traceRay(ray, camera->getTMin(), 0, 1.f, 1.f, hit);
}

void renderFunc() {
    auto imgColor = Image(CmdlParser->width, CmdlParser->height);
    auto imgNormal = Image(CmdlParser->width, CmdlParser->height);
    auto imgDepth = Image(CmdlParser->width, CmdlParser->height);

    imgColor.SetAllPixels(Vec3f(0.f, 0.f, 0.f));
    imgNormal.SetAllPixels(Vec3f(0.f, 0.f, 0.f));
    imgDepth.SetAllPixels(Vec3f(0.f, 0.f, 0.f));

    auto *camera = ScParser->getCamera();
    assert(camera != nullptr);

    for (int y = 0; y < CmdlParser->width; ++y) {
        for (int x = 0; x < CmdlParser->height; ++x) {
            Vec2f p(float(x) / CmdlParser->width, float(y) / CmdlParser->height);
            auto ray = camera->generateRay(p);
            Hit hit;
            auto color = RTracer->traceRay(ray, camera->getTMin(), 0, 1.f, 1.f, hit);

            if (CmdlParser->output_file != nullptr) {
                imgColor.SetPixel(x, y, color);
            }

            if (CmdlParser->depth_file != nullptr) {
                auto t = hit.getT();
                t = std::min(CmdlParser->depth_max, t);
                t = std::max(CmdlParser->depth_min, t);
                float weight = (t - CmdlParser->depth_min) / (CmdlParser->depth_max - CmdlParser->depth_min);
                Vec3f depthColor;
                Vec3f::WeightedSum(depthColor, Vec3f(0.f, 0.f, 0.f), weight, Vec3f(1.f, 1.f, 1.f), 1 - weight);
                imgDepth.SetPixel(x, y, depthColor);
            }

            if (CmdlParser->normal_file != nullptr) {
                auto norm = hit.getNormal();
                Vec3f normalColor = Vec3f(fabsf(norm.x()), fabsf(norm.y()), fabsf(norm.z()));
                imgNormal.SetPixel(x, y, normalColor);
            }
        }
    }

    if (CmdlParser->output_file != nullptr) {
        imgColor.SaveTGA(CmdlParser->output_file);
    }

    if (CmdlParser->depth_file != nullptr) {
        imgDepth.SaveTGA(CmdlParser->depth_file);
    }

    if (CmdlParser->normal_file != nullptr) {
        imgNormal.SaveTGA(CmdlParser->normal_file);
    }
}

int main(int argc, char* argv[]) {
    CmdlParser = std::make_shared<CommandLineArgumentParser>();
    CmdlParser->parse(argc, argv);
    ScParser = std::make_shared<SceneParser>(CmdlParser->input_file);
    RTracer = std::make_shared<RayTracer>(ScParser, CmdlParser);
    if (CmdlParser->use_gui) {
        GLCanvas canvas;
        canvas.initialize(ScParser.get(), renderFunc, traceRayFunc, RTracer->getGrid(), CmdlParser->visualize_grid);
    } else {
        renderFunc();
    }
}

// main -input ../assets/scene5_01_sphere.txt -size 200 200 -output output5_01a.tga -grid 1 1 1 -visualize_grid -gui
// main -input ../assets/scene5_01_sphere.txt -size 200 200 -output output5_01b.tga -grid 5 5 5 -visualize_grid -gui
// main -input ../assets/scene5_01_sphere.txt -size 200 200 -output output5_01c.tga -grid 15 15 15 -visualize_grid -gui

// main -input ../assets/scene5_02_spheres.txt -size 200 200 -output output5_02a.tga -gui -grid 15 15 15 -visualize_grid
// main -input ../assets/scene5_02_spheres.txt -size 200 200 -output output5_02b.tga -gui -grid 15 15 3 -visualize_grid

// 按 g 来查看光线与 boundingbox 相交的面
// main -input ../assets/scene5_02_spheres.txt -size 200 200 -gui -grid 8 8 8 -visualize_grid

// main -input ../assets/scene5_03_offcenter_spheres.txt -size 200 200 -output output5_03.tga -gui -grid 20 20 20 -visualize_grid

// main -input ../assets/scene5_04_plane_test.txt -size 200 200 -gui -tessellation 30 15 -gouraud
// main -input ../assets/scene5_04_plane_test.txt -size 200 200 -output output5_04.tga -gui -grid 15 15 15 -visualize_grid

// main -input ../assets/scene5_05_sphere_triangles.txt -size 200 200 -output output5_05b.tga -gui -tessellation 30 15 -gouraud
// main -input ../assets/scene5_05_sphere_triangles.txt -size 200 200 -output output5_05.tga -gui -grid 20 20 10 -visualize_grid

// main -input ../assets/scene5_06_bunny_mesh_200.txt -size 200 200 -output output5_06.tga -gui -grid 10 10 7 -visualize_grid
// main -input ../assets/scene5_07_bunny_mesh_1k.txt -size 200 200 -output output5_07.tga -gui -grid 15 15 12 -visualize_grid
// main -input ../assets/scene5_08_bunny_mesh_5k.txt -size 200 200 -output output5_08.tga -gui -grid 20 20 15 -visualize_grid
// main -input ../assets/scene5_09_bunny_mesh_40k.txt -size 200 200 -output output5_09.tga -gui -grid 40 40 33 -visualize_grid

// main -input ../assets/scene5_10_scale_translate.txt -size 200 200 -gui -output output5_10a.tga -tessellation 30 15 -gouraud
// main -input ../assets/scene5_10_scale_translate.txt -size 200 200 -output output5_10.tga -gui -grid 15 15 15 -visualize_grid

// main -input ../assets/scene5_11_rotated_triangles.txt -size 200 200 -output output5_11a.tga -gui
// main -input ../assets/scene5_11_rotated_triangles.txt -size 200 200 -output output5_11.tga -gui -grid 15 15 9 -visualize_grid

// main -input ../assets/scene5_12_nested_transformations.txt -size 200 200 -output output5_12a.tga -gui
// main -input ../assets/scene5_12_nested_transformations.txt -size 200 200 -output output5_12.tga -gui -grid 30 30 30 -visualize_grid





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
#include "film.h"
#include "sampler.h"
#include "filter.h"
#include <stdlib.h>
#include <time.h>

std::shared_ptr<CommandLineArgumentParser> CmdlParser;
std::shared_ptr<SceneParser> ScParser;
std::shared_ptr<RayTracer> RTracer;

void traceRayFunc(float x, float y)
{
    auto *camera = ScParser->getCamera();
    assert(camera != nullptr);
    Vec2f p(x, y);
    auto ray = camera->generateRay(p);
    Hit hit;
    RTracer->getGrid()->refreshColorSchema();
    RTracer->traceRay(ray, camera->getTMin(), 0, 1.f, 1.f, hit);
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

    Film *film;
    Sampler *sampler;
    Filter *filter;

    if (CmdlParser->sampler_strategy != nullptr)
    {
        film = new Film(CmdlParser->width, CmdlParser->height, CmdlParser->samples_num);
        sampler = Sampler::GetSamplerByStrategy(CmdlParser->sampler_strategy, CmdlParser->samples_num);
    }
    else
    {
        film = new Film(CmdlParser->width, CmdlParser->height, 1);
        sampler = Sampler::GetDefaultSampler();
    }

    if (CmdlParser->filter_strategy != nullptr)
    {
        filter = Filter::GetFilterByStrategy(CmdlParser->filter_strategy, CmdlParser->filter_argument);
    }
    else
    {
        filter = Filter::GetDefaultFilter();
    }

    for (int x = 0; x < CmdlParser->width; ++x)
    {
        for (int y = 0; y < CmdlParser->height; ++y)
        {
            Hit hit;
            float maxImageSize = std::max(CmdlParser->width, CmdlParser->height);
            float xMargin = (maxImageSize - CmdlParser->width) / 2.f;
            float yMargin = (maxImageSize - CmdlParser->height) / 2.f;
            Vec2f pixelPos((float(x) + xMargin) / maxImageSize, (float(y) + yMargin) / maxImageSize);
            Vec2f pixelSize(1.f / CmdlParser->width, 1.f / CmdlParser->height);
            for (int i = 0; i < film->getNumSamples(); ++i)
            {
                hit = Hit();
                Vec2f samplePos = sampler->getSamplePosition(i);
                auto ray = camera->generateRay(pixelPos + samplePos * pixelSize);
                auto color = RTracer->traceRay(ray, camera->getTMin(), 0, 1.f, 1.f, hit);
                film->setSample(x, y, i, samplePos, color);
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
        for (int x = 0; x < CmdlParser->width; ++x)
        {
            for (int y = 0; y < CmdlParser->height; ++y)
            {
                imgColor.SetPixel(x, y, filter->getColor(x, y, film));
            }
        }
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
    if (CmdlParser->samples_file != nullptr)
    {
        film->renderSamples(CmdlParser->samples_file, CmdlParser->samples_zoom);
    }
    if (CmdlParser->filter_file != nullptr)
    {
        film->renderFilter(CmdlParser->filter_file, CmdlParser->filter_zoom, filter);
    }

    if (CmdlParser->stats)
        RayTracingStats::PrintStatistics();
}

int main(int argc, char *argv[])
{
    srand(time(nullptr));
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

// main -input ../assets/scene7_01_sphere_triangle.txt -size 180 180 -output output7_01.tga

// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01a.tga 20 -random_samples 4
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01b.tga 20 -uniform_samples 4
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01c.tga 20 -jittered_samples 4
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01d.tga 20 -random_samples 9
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01e.tga 20 -uniform_samples 9
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01f.tga 20 -jittered_samples 9
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01g.tga 20 -random_samples 36
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01h.tga 20 -uniform_samples 36
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_samples samples7_01i.tga 20 -jittered_samples 36

// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01a.tga 20 -box_filter 0.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01b.tga 20 -tent_filter 0.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01c.tga 20 -gaussian_filter 0.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01d.tga 20 -box_filter 1.7
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01e.tga 20 -tent_filter 1.7
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01f.tga 20 -gaussian_filter 1.7
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01g.tga 20 -box_filter 2.3
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01h.tga 20 -tent_filter 2.3
// main -input ../assets/scene7_01_sphere_triangle.txt -size 9 9 -render_filter filter7_01i.tga 20 -gaussian_filter 2.3

// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01_low_res.tga
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -render_samples samples7_01a_low_res.tga 15 -random_samples 9
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -render_samples samples7_01b_low_res.tga 15 -uniform_samples 9
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -render_samples samples7_01c_low_res.tga 15 -jittered_samples 9
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -render_filter filter7_01a_low_res.tga 15 -box_filter 0.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -render_filter filter7_01b_low_res.tga 15 -tent_filter 1.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -render_filter filter7_01c_low_res.tga 15 -gaussian_filter 1.0
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01a_low_res.tga -random_samples 9 -box_filter 0.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01b_low_res.tga -random_samples 9 -tent_filter 1.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01c_low_res.tga -random_samples 9 -gaussian_filter 1.0
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01d_low_res.tga -uniform_samples 9 -box_filter 0.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01e_low_res.tga -uniform_samples 9 -tent_filter 1.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01f_low_res.tga -uniform_samples 9 -gaussian_filter 1.0
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01g_low_res.tga -jittered_samples 9 -box_filter 0.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01h_low_res.tga -jittered_samples 9 -tent_filter 1.5
// main -input ../assets/scene7_01_sphere_triangle.txt -size 12 12 -output output7_01i_low_res.tga -jittered_samples 9 -gaussian_filter 1.0

// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02.tga
// main -input ../assets/scene7_02_checkerboard.txt -size 9 9 -render_samples samples7_02a.tga 20 -random_samples 16
// main -input ../assets/scene7_02_checkerboard.txt -size 9 9 -render_samples samples7_02b.tga 20 -uniform_samples 16
// main -input ../assets/scene7_02_checkerboard.txt -size 9 9 -render_samples samples7_02c.tga 20 -jittered_samples 16
// main -input ../assets/scene7_02_checkerboard.txt -size 9 9 -render_filter filter7_02a.tga 20 -box_filter 0.5
// main -input ../assets/scene7_02_checkerboard.txt -size 9 9 -render_filter filter7_02b.tga 20 -tent_filter 1.5
// main -input ../assets/scene7_02_checkerboard.txt -size 9 9 -render_filter filter7_02c.tga 20 -gaussian_filter 0.6
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02a.tga -random_samples 16 -box_filter 0.5
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02b.tga -random_samples 16 -tent_filter 1.5
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02c.tga -random_samples 16 -gaussian_filter 0.6
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02d.tga -uniform_samples 16 -box_filter 0.5
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02e.tga -uniform_samples 16 -tent_filter 1.5
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02f.tga -uniform_samples 16 -gaussian_filter 0.6
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02g.tga -jittered_samples 16 -box_filter 0.5
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02h.tga -jittered_samples 16 -tent_filter 1.5
// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02i.tga -jittered_samples 16 -gaussian_filter 0.6

// main -input ../assets/scene7_02_checkerboard.txt -size 180 180 -output output7_02i_test.tga -jittered_samples 16 -gaussian_filter 0.6

// main -input ../assets/scene7_03_marble_vase.txt  -size 200 200 -output output7_03a.tga -grid 15 30 15 -shadows
// main -input ../assets/scene7_03_marble_vase.txt  -size 200 200 -output output7_03b.tga -grid 15 30 15 -shadows -jittered_samples 4 -gaussian_filter 0.4
// main -input ../assets/scene7_03_marble_vase.txt  -size 200 200 -output output7_03c.tga -grid 15 30 15 -shadows -jittered_samples 9 -gaussian_filter 0.4
// main -input ../assets/scene7_03_marble_vase.txt  -size 200 200 -output output7_03d.tga -grid 15 30 15 -shadows -jittered_samples 36 -gaussian_filter 0.4

// main -input ../assets/scene7_04_6.837_logo.txt   -size 400 200 -output output7_04a.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 80 30 3
// main -input ../assets/scene7_04_6.837_logo.txt   -size 400 200 -output output7_04b.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 80 30 3 -jittered_samples 9 -gaussian_filter 0.4
// main -input ../assets/scene7_04_6.837_logo.txt   -size 400 200 -output output7_04b_test.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 80 30 3 -jittered_samples 9 -gaussian_filter 0.4

// main -input ../assets/scene7_05_glass_sphere.txt -size 300 300 -output output7_05a.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20
// main -input ../assets/scene7_05_glass_sphere.txt -size 300 300 -output output7_05b.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20 -jittered_samples 4 -gaussian_filter 0.4
// main -input ../assets/scene7_05_glass_sphere.txt -size 300 300 -output output7_05c.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20 -jittered_samples 16 -gaussian_filter 0.4

// main -input ../assets/scene7_06_faceted_gem.txt -size 200 200 -output output7_06a.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20 
// main -input ../assets/scene7_06_faceted_gem.txt -size 200 200 -output output7_06b.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20 -jittered_samples 9 -gaussian_filter 0.4
// main -input ../assets/scene7_06_faceted_gem.txt -size 200 200 -output output7_06b_test.tga -shadows -shade_back -bounces 5 -weight 0.01 -grid 20 20 20 -jittered_samples 9 -gaussian_filter 0.4

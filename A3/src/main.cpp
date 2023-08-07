#include "commandLineArgumentParser.h"
#include "image.h"
#include "raytracer.h"
#include "glCanvas.h"

CommandLineArgumentParser cmdlParser;
RayTracer rayTracer;

void renderFunc() {
    auto img = Image(cmdlParser.width, cmdlParser.height);
    rayTracer.renderToImage(img, cmdlParser.shade_back);
    img.SaveTGA(cmdlParser.output_file);

    if (cmdlParser.depth_file != nullptr) {
        rayTracer.renderDepthToImage(img, cmdlParser.depth_min, cmdlParser.depth_max);
        img.SaveTGA(cmdlParser.depth_file);
    }

    if (cmdlParser.normal_file != nullptr) {
        rayTracer.renderNormalToImage(img);
        img.SaveTGA(cmdlParser.normal_file);
    }
}

int main(int argc, char* argv[]) {
    cmdlParser.parse(argc, argv);

    rayTracer.readSceneFromFile(cmdlParser.input_file);

    if (cmdlParser.use_gui) {
        GLCanvas canvas;
        canvas.initialize(rayTracer.getSceneParser().get(), renderFunc);
    } else {
        renderFunc();
    }
}

// main -input ../assets/scene3_01_cube_orthographic.txt -size 200 200 -output output3_01.tga -gui

// main -input ../assets/scene3_02_cube_perspective.txt -size 200 200 -output output3_02.tga -gui

// main -input ../assets/scene3_03_bunny_mesh_200.txt -size 200 200 -output output3_03.tga -gui
// main -input ../assets/scene3_04_bunny_mesh_1k.txt -size 200 200 -output output3_04.tga -gui

// main -input ../assets/scene3_05_axes_cube.txt -size 200 200 -output output3_05.tga -gui

// main -input ../assets/scene3_06_crazy_transforms.txt -size 200 200 -output output3_06.tga -gui

// main -input ../assets/scene3_07_plane.txt -size 200 200 -output output3_07.tga -gui -tessellation 10 5

// main -input ../assets/scene3_08_sphere.txt -size 200 200 -output output3_08.tga -gui -tessellation 10 5
// main -input ../assets/scene3_08_sphere.txt -size 200 200 -output output3_08.tga -gui -tessellation 20 10
// main -input ../assets/scene3_08_sphere.txt -size 200 200 -output output3_08.tga -gui -tessellation 10 5 -gouraud
// main -input ../assets/scene3_08_sphere.txt -size 200 200 -output output3_08.tga -gui -tessellation 20 10 -gouraud

// main -input ../assets/scene3_09_exponent_variations.txt -size 300 300 -output output3_09.tga -gui -tessellation 100 50 -gouraud

// main -input ../assets/scene3_10_exponent_variations_back.txt -size 300 300 -output output3_10.tga -gui -tessellation 100 50 -gouraud

// main -input ../assets/scene3_11_weird_lighting_diffuse.txt -size 200 200 -output output3_11.tga -gui -tessellation 100 50 -gouraud

// main -input ../assets/scene3_12_weird_lighting_specular.txt -size 200 200 -output output3_12.tga -gui -tessellation 100 50 -gouraud

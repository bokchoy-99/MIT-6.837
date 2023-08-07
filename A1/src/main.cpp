
#include <cstring>
#include <cassert>
#include <cstdio>

#include "image.h"
#include "raytracer.h"

// main -input ../data/scene1_02.txt -size 200 200 -output output1_02.tga -depth 8 12 depth1_02.tga

int main(int argc, char* argv[]) {
    char *input_file = nullptr;
    int width = 100;
    int height = 100;
    char *output_file = nullptr;
    float depth_min = 0;
    float depth_max = 1;
    char *depth_file = nullptr;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc);
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-size")) {
            i++; assert (i < argc);
            width = atoi(argv[i]);
            i++; assert (i < argc);
            height = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc);
            output_file = argv[i];
        } else if (!strcmp(argv[i],"-depth")) {
            i++; assert (i < argc);
            depth_min = atof(argv[i]);
            i++; assert (i < argc);
            depth_max = atof(argv[i]);
            i++; assert (i < argc);
            depth_file = argv[i];
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

    auto img = Image(width, height);
    RayTracer rayTracer;
    rayTracer.readSceneFromFile(input_file);
    rayTracer.renderToImage(img);
    img.SaveTGA(output_file);

    rayTracer.renderDepthToImage(img, depth_min, depth_max);
    img.SaveTGA(depth_file);
}

// main -input ../assets/scene1_01.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga
// main -input ../assets/scene1_02.txt -size 200 200 -output output1_02.tga -depth 8 12 depth1_02.tga
// main -input ../assets/scene1_03.txt -size 200 200 -output output1_03.tga -depth 8 12 depth1_03.tga
// main -input ../assets/scene1_04.txt -size 200 200 -output output1_04.tga -depth 12 17 depth1_04.tga
// main -input ../assets/scene1_05.txt -size 200 200 -output output1_05.tga -depth 14.5 19.5 depth1_05.tga
// main -input ../assets/scene1_06.txt -size 200 200 -output output1_06.tga -depth 3 7 depth1_06.tga
// main -input ../assets/scene1_07.txt -size 200 200 -output output1_07.tga -depth -2 2 depth1_07.tga

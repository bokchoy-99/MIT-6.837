#include <cstdlib>
#include <cassert>
#include <cstring>
#include "image.h"
#include "matrix.h"
#include "ifs.h"

int main(int argc, char* argv[]){
    //HANDLE COMMAND LINE ARGUMENTS
    //sample command line:
    //main -input ../data/fern.txt -points 10000 -iters 10 -size 100 -output fern.tga
    //main -input ../data/dragon.txt -points 10000 -iters 10 -size 100 -output dragon.tga
    //main -input ../data/giant_x.txt -points 10000 -iters 10 -size 100 -output giant_x.tga
    char *input_file(NULL), *output_file(NULL);
    int num_points(10000), num_iters(10), size(100);

    for (int i = 1; i < argc; i++){
        if (!strcmp(argv[i], "-input")){
            i++; assert(i < argc);
            input_file = argv[i];
        } else if(!strcmp(argv[i], "-points")){
            i++; assert(i < argc);
            num_points = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-iters")){
            i++; assert(i < argc);
            num_iters = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-size")){
            i++; assert(i < argc);
            size = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-output")){
            i++; assert(i < argc);
            output_file = argv[i];
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

    // // ifs -input ../data/fern.txt -points 10000 -iters 10 -size 100 -output fern.tga
    // input_file = (char*)"data/fern.txt";
    // num_points = 10000;
    // size = 100;
    // num_iters = 10;
    // output_file = (char*)"fern.tga";
    
    //Image instance
    Image img(size, size);
    img.SetAllPixels(Vec3f(1.0, 1.0, 1.0));
    //Ifs instance
    Ifs ifs(input_file, num_points, num_iters);
    ifs.render(img);
    const char *ext = &output_file[strlen(output_file)-4];
    if (!strcmp(ext, ".ppm")) img.SavePPM(output_file);
    if (!strcmp(ext, ".tga")) img.SaveTGA(output_file);

    return 0;
}

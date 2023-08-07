#include <stdio.h>
#include <assert.h>
using namespace std;

#include "arg_parser.h"
#include "glCanvas.h"
#include "spline_parser.h"

// ====================================================================
// ====================================================================

int main(int argc, char *argv[])
{

    // parse the command line arguments & input file
    ArgParser *args = new ArgParser(argc, argv);
    SplineParser *splines = new SplineParser(args->input_file);

    // launch curve editor!
    if (args->gui)
    {
        GLCanvas glcanvas;
        glcanvas.initialize(args, splines);
        // this never returns...
    }

    // output as required
    splines->SaveBezier(args);
    splines->SaveBSpline(args);
    splines->SaveTriangles(args);

    // cleanup
    delete args;
    delete splines;
    return 0;
}

// ====================================================================
// ====================================================================

// main -input ../assets/spline8_01_bezier.txt -gui -curve_tessellation 30
// main -input ../assets/spline8_02_bspline.txt -gui -curve_tessellation 30

// main -input ../assets/spline8_01_bezier.txt -output_bezier output8_01_bezier.txt
// main -input ../assets/spline8_01_bezier.txt -output_bspline output8_01_bspline.txt
// main -input ../assets/spline8_02_bspline.txt -output_bezier output8_02_bezier.txt
// main -input ../assets/spline8_02_bspline.txt -output_bspline output8_02_bspline.txt
// main -input ../assets/output8_01_bezier.txt -gui -curve_tessellation 30
// main -input ../assets/output8_01_bspline.txt -gui -curve_tessellation 30
// main -input ../assets/output8_02_bezier.txt -gui -curve_tessellation 30
// main -input ../assets/output8_02_bspline.txt -gui -curve_tessellation 30

// main -input ../assets/spline8_03_bezier.txt -gui -curve_tessellation 30
// main -input ../assets/spline8_04_bspline.txt -gui -curve_tessellation 30
// main -input ../assets/spline8_05_bspline_dups.txt -gui -curve_tessellation 30




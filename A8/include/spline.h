#ifndef _SPLINE_H
#define _SPLINE_H

#include <vector>

#include "arg_parser.h"
#include "vectors.h"
#include "triangle_mesh.h"

class Spline
{
protected:
    int nVertices;
    std::vector<Vec3f> ctrlPoints;

public:
    explicit Spline(int _nVertices);

    virtual void set(int i, const Vec3f &v);

    // FOR VISUALIZATION
    virtual void Paint(ArgParser *args) { assert(0); }

    // FOR CONVERTING BETWEEN SPLINE TYPES
    virtual void OutputBezier(FILE *file) { assert(0); }
    virtual void OutputBSpline(FILE *file) { assert(0); }

    // FOR CONTROL POINT PICKING
    virtual int getNumVertices() { return nVertices; }
    virtual Vec3f getVertex(int i) { return ctrlPoints[i]; }

    // FOR EDITING OPERATIONS
    virtual void moveControlPoint(int selectedPoint, float x, float y);
    virtual void addControlPoint(int selectedPoint, float x, float y);
    virtual void deleteControlPoint(int selectedPoint);

    // FOR GENERATING TRIANGLES
    virtual TriangleMesh *OutputTriangles(ArgParser *args)
    {
        assert(0);
        return nullptr;
    }
};

#endif 

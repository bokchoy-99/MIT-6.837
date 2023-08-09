#ifndef _SURFACE_H
#define _SURFACE_H

#include <memory>

#include "spline.h"
#include "curve.h"

class Surface : public Spline
{
public:
    explicit Surface(int nVertices) : Spline(nVertices) {}
};

class SurfaceOfRevolution : public Surface
{
    std::shared_ptr<Curve> curve;

public:
    explicit SurfaceOfRevolution(Curve *c);

    void set(int i, const Vec3f &v) override { curve->set(i, v); }

    void Paint(ArgParser *args) override;

    int getNumVertices() override { return curve->getNumVertices(); }
    Vec3f getVertex(int i) override { return curve->getVertex(i); }

    void moveControlPoint(int selectedPoint, float x, float y) override { curve->moveControlPoint(selectedPoint, x, y); }
    void addControlPoint(int selectedPoint, float x, float y) override { curve->addControlPoint(selectedPoint, x, y); }
    void deleteControlPoint(int selectedPoint) override { curve->deleteControlPoint(selectedPoint); }

    void OutputBezier(FILE *file) override;
    void OutputBSpline(FILE *file) override;
    TriangleMesh *OutputTriangles(ArgParser *args) override;
};

class BezierPatch : public Surface
{
    std::shared_ptr<BezierCurve> ctrlCurves;

public:
    BezierPatch();

    void set(int i, const Vec3f &v) override;

    void Paint(ArgParser *args) override;

    TriangleMesh *OutputTriangles(ArgParser *args) override;
};

#endif // ASSIGNMENTS_SURFACE_H

#ifndef _CURVE_H
#define _CURVE_H

#include <memory>
#include <string>
#include <vector>
#include "spline.h"
#include "matrix.h"

class CurveWindow
{
public:
    Matrix G, *B;
    Vec3f getPointAtT(float t) const;
};

class Curve : public Spline
{
protected:
    vector<CurveWindow> windows;
    void makeWindow(int windowIndex, int beginPointIndex);
    string type;
    void output(FILE *file);

public:
    explicit Curve(int _nVertices) : Spline(_nVertices) {}

    virtual Matrix *getB() = 0;
    vector<CurveWindow> getWindows();

    void Paint(ArgParser *args);
    
    virtual void makeWindows() = 0;
};

class BezierCurve;
class BSplineCurve;

class BezierCurve : public Curve
{
    static Matrix B;
    void makeWindows();

    friend class BezierPatch;
    BezierCurve() : Curve(0) {}

public:
    explicit BezierCurve(int _nVertices);
    Matrix *getB() { return &B; }

    BSplineCurve *toBSpline();

    void OutputBezier(FILE *file);
    void OutputBSpline(FILE *file);
};

class BSplineCurve : public Curve
{
    static Matrix B;
    void makeWindows();

public:
    explicit BSplineCurve(int _nVertices);
    Matrix *getB() { return &B; }

    BezierCurve *toBezier();

    void OutputBezier(FILE *file) override;
    void OutputBSpline(FILE *file) override;
};

#endif


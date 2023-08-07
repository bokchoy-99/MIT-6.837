#include "curve.h"

#include <GL/gl.h>

Vec3f CurveWindow::getPointAtT(float t) const
{
    auto res = Vec4f(t * t * t, t * t, t, 1);
    (G * *B).Transform(res);
    res.DivideByW();
    return res.xyz();
}

void Curve::Paint(ArgParser *args)
{
    // 绘制控制点之间的连线
    glColor3f(0.f, 0.f, 1.f);
    glLineWidth(2.f);
    glBegin(GL_LINE_STRIP);
    for (auto &p : ctrlPoints)
    {
        glVertex3fv(p.GetData());
    }
    glEnd();

    // 绘制曲线
    makeWindows();
    int tess = args->curve_tessellation;
    glColor3f(0.f, 1.f, 0.f);
    glLineWidth(3.f);
    glBegin(GL_LINE_STRIP);
    for (const auto &win : windows)
    {
        for (int i = 0; i <= tess; i++)
        {
            glVertex3fv(win.getPointAtT(1.f / tess * i).GetData());
        }
    }
    glEnd();

    // 绘制控制点
    glColor3f(1.f, 0.f, 0.f);
    glPointSize(4.f);
    glBegin(GL_POINTS);
    for (auto &p : ctrlPoints)
    {
        glVertex3fv(p.GetData());
    }
    glEnd();
}

void Curve::makeWindow(int windowIndex, int beginPointIndex)
{
    auto &win = windows[windowIndex];
    win.B = getB();
    Vec4f v[4];
    for (int i = 0; i < 4; i++)
    {
        v[i] = Vec4f(ctrlPoints[beginPointIndex + i], 1.f);
    }
    win.G = Matrix(v);
}

void Curve::output(FILE *file)
{
    fprintf(file, "%s\nnum_vertices %d\n", type.c_str(), nVertices);
    for (const auto &p : ctrlPoints)
    {
        fprintf(file, "%.2f %.2f %.2f\n", p.x(), p.y(), p.z());
    }
}

vector<CurveWindow> Curve::getWindows()
{
    makeWindows();
    return windows;
}

// --------------------------------------------------------------
// --------------------------------------------------------------

static float BezierBFloats[16] = {
    -1.f, 3.f, -3.f, 1.f,
    3.f, -6.f, 3.f, 0.f,
    -3.f, 3.f, 0.f, 0.f,
    1.f, 0.f, 0.f, 0.f};

Matrix BezierCurve::B = Matrix(BezierBFloats);

BezierCurve::BezierCurve(int _nVertices) : Curve(_nVertices)
{
    type = "bezier";
}

void BezierCurve::makeWindows()
{ 
    int num_win = (nVertices - 1) / 3;
    windows.resize(static_cast<unsigned long>(num_win));
    for (int i = 0; i < num_win; i++)
    {
        int beginPointIndex = i * 3;
        makeWindow(i, beginPointIndex);
    }
}

BSplineCurve *BezierCurve::toBSpline()
{
    // ???
}

void BezierCurve::OutputBezier(FILE *file)
{
    output(file);
}

void BezierCurve::OutputBSpline(FILE *file)
{
    toBSpline()->OutputBSpline(file);
}

// --------------------------------------------------------------
// --------------------------------------------------------------

static float BSplineBFloats[16] = {
    -1.f, 3.f, -3.f, 1.f,
    3.f, -6.f, 0.f, 4.f,
    -3.f, 3.f, 3.f, 1.f,
    1.f, 0.f, 0.f, 0.f};

Matrix BSplineCurve::B = Matrix(BSplineBFloats);

BSplineCurve::BSplineCurve(int _nVertices) : Curve(_nVertices)
{
    type = "bspline";
}

void BSplineCurve::makeWindows()
{
    int num_win = nVertices - 3;
    windows.resize(static_cast<unsigned long>(num_win));
    for (int i = 0; i < num_win; i++)
    {
        int beginPointIndex = i;
        makeWindow(i, beginPointIndex);
    }
}

BezierCurve *BSplineCurve::toBezier()
{
    // ???
}

void BSplineCurve::OutputBezier(FILE *file)
{
    toBezier()->OutputBezier(file);
}

void BSplineCurve::OutputBSpline(FILE *file)
{
    output(file);
}

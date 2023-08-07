
#include "hsl.h"

HSLColor::HSLColor(float _h, float _s, float _l) : h(_h), s(_s), l(_l) {}

Vec3f HSLColor::toRGBColor()
{
    float r, g, b;
    if (s == 0.0f)
    {
        r = g = b = l;
    }
    else
    {
        float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        float hk = h / 360;
        float tr = hk + (float)1 / 3, tg = hk, tb = hk - (float)1 / 3;
        tr = cvttf1(tr);
        tg = cvttf1(tg);
        tb = cvttf1(tb);
        tr = cvttf2(tr);
        tg = cvttf2(tg);
        tb = cvttf2(tb);
        r = cvttf3(tr, p, q);
        g = cvttf3(tg, p, q);
        b = cvttf3(tb, p, q);
    }

    return Vec3f(r, g, b);
}

float HSLColor::cvttf1(float in)
{
    if (in < 0)
        in += 1.0;
    return in;
}

float HSLColor::cvttf2(float in)
{
    if (in > 1)
        in -= 1.0;
    return in;
}

float HSLColor::cvttf3(float in, float p, float q)
{
    if (in < (float)1 / 6)
        return p + ((q - p) * 6 * in);
    else if (in < (float)1 / 2)
        return q;
    else if (in < (float)2 / 3)
        return p + ((q - p) * 6 * ((float)2 / 3 - in));
    else
        return p;
}

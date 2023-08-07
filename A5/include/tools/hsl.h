#ifndef _HSL_H
#define _HSL_H

#include "vectors.h"

struct HSLColor {
    float h, s, l;

    float cvttf1(float in);
    float cvttf2(float in);
    float cvttf3(float in, float p, float q);

public:
    HSLColor(float _h, float _s, float _l);

    Vec3f toRGBColor();
};

#endif //ASSIGNMENTS_HSL_H

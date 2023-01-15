#include "light.h"

#include <algorithm>

/* Solves a quadratic equation
 *
 * a,b,c - coefficients of the equation
 * x1,x2 - pointers used to return the results. May be null.
 *
 * If the equation has 1 root, then both x1 and x2 will be set to it.
 *
 * If the equation has 2 roots, then x1 will be set to the smaller and x2
 * to the larger root.
 *
 * Return value is the number of roots.
 */
int SolveQuadratic(float a, float b, float c, float *x1, float *x2)
{
    float delta = b*b - 4*a*c;

    if(std::abs(a) < 1e-6f)
    {
        if(std::abs(b) > 1e-6f)
        {
            if(x1) *x1 = -c/b;
            if(x2) *x2 = -c/b;
            return 1;
        } else return 0;
    }
    else if(delta > 1e-6f)
    {
        if(x1) *x1 = (-b-(float)sqrt(delta))/(2*a);
        if(x2) *x2 = (-b+(float)sqrt(delta))/(2*a);
        return 2;
    }
    else if(delta >= 0)
    {
        if(x1) *x1 = -b/(2*a);
        if(x2) *x2 = -b/(2*a);
        return 1;
    }
    else return 0;
}

namespace Game {

    float Light::Range() const {

        float brightest_component = std::max(intensity.r, std::max(intensity.g, intensity.b));
        float range;
        if(!SolveQuadratic(
            kQuadratic * kBias,
            kLinear * kBias,
            kBias - brightest_component,
            nullptr, &range
        )) range = 0;
        return range;
    }
    void Light::SetRange(float newRange) {
        float ratio = newRange / Range();
        if(ratio > 0) {
            kQuadratic /= ratio*ratio;
            kLinear /= ratio;
        }
    }

    LightRenderer::LightRenderer() /*:
        program(
            R"glsl(
                #version 330 core

                void main() {

                }
            )glsl",
            R"glsl(
                #version 330 core

                void main() {
                    
                }
            )glsl"
        )*/
    {}
}
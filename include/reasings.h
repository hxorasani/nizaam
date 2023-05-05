/*
*
*   reasings - raylib easings library, based on Robert Penner library
*
*   Useful easing functions for values animation
*
*   This header uses:
*       #define REASINGS_STATIC_INLINE      // Inlines all functions code, so it runs faster.
*                                           // This requires lots of memory on system.
*   How to use:
*   The four inputs t,b,c,d are defined as follows:
*   t = current time (in any unit measure, but same unit as duration)
*   b = starting value to interpolate
*   c = the total change in value of b that needs to occur
*   d = total time it should take to complete (duration)
*
*   Example:
*
*   int currentTime = 0;
*   int duration = 100;
*   double startPositionX = 0.0f;
*   double finalPositionX = 30.0f;
*   double currentPositionX = startPositionX;
*
*   while (currentPositionX < finalPositionX)
*   {
*       currentPositionX = EaseSineIn(currentTime, startPositionX, finalPositionX - startPositionX, duration);
*       currentTime++;
*   }
*
*   A port of Robert Penner's easing equations to C (http://robertpenner.com/easing/)
*
*   Robert Penner License
* */

#ifndef REASINGS_H
#define REASINGS_H

#define REASINGS_STATIC_INLINE     // NOTE: By default, compile functions as static inline

#if defined(REASINGS_STATIC_INLINE)
    #define EASEDEF static inline
#else
    #define EASEDEF extern
#endif

#include <math.h>       // Required for: sinf(), cosf(), sqrtf(), powf()

#ifndef PI
    #define PI 3.14159265358979323846f //Required as PI is not always defined in math.h
#endif

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

// Linear Easing functions
EASEDEF double EaseLinearNone(double t, double b, double c, double d) { return (c*t/d + b); }                            // Ease: Linear
EASEDEF double EaseLinearIn(double t, double b, double c, double d) { return (c*t/d + b); }                              // Ease: Linear In
EASEDEF double EaseLinearOut(double t, double b, double c, double d) { return (c*t/d + b); }                             // Ease: Linear Out
EASEDEF double EaseLinearInOut(double t, double b, double c, double d) { return (c*t/d + b); }                           // Ease: Linear In Out

// Sine Easing functions
EASEDEF double EaseSineIn(double t, double b, double c, double d) { return (-c*cosf(t/d*(PI/2.0f)) + c + b); }            // Ease: Sine In
EASEDEF double EaseSineOut(double t, double b, double c, double d) { return (c*sinf(t/d*(PI/2.0f)) + b); }                // Ease: Sine Out
EASEDEF double EaseSineInOut(double t, double b, double c, double d) { return (-c/2.0f*(cosf(PI*t/d) - 1.0f) + b); }      // Ease: Sine Out

// Circular Easing functions
EASEDEF double EaseCircIn(double t, double b, double c, double d) { t /= d; return (-c*(sqrtf(1.0f - t*t) - 1.0f) + b); } // Ease: Circular In
EASEDEF double EaseCircOut(double t, double b, double c, double d) { t = t/d - 1.0f; return (c*sqrtf(1.0f - t*t) + b); }  // Ease: Circular Out
EASEDEF double EaseCircInOut(double t, double b, double c, double d)                                                      // Ease: Circular In Out
{
    if ((t/=d/2.0f) < 1.0f) return (-c/2.0f*(sqrtf(1.0f - t*t) - 1.0f) + b);
    t -= 2.0f; return (c/2.0f*(sqrtf(1.0f - t*t) + 1.0f) + b);
}

// Cubic Easing functions
EASEDEF double EaseCubicIn(double t, double b, double c, double d) { t /= d; return (c*t*t*t + b); }                      // Ease: Cubic In
EASEDEF double EaseCubicOut(double t, double b, double c, double d) { t = t/d - 1.0f; return (c*(t*t*t + 1.0f) + b); }    // Ease: Cubic Out
EASEDEF double EaseCubicInOut(double t, double b, double c, double d)                                                     // Ease: Cubic In Out
{
    if ((t/=d/2.0f) < 1.0f) return (c/2.0f*t*t*t + b);
    t -= 2.0f; return (c/2.0f*(t*t*t + 2.0f) + b);
}

// Quadratic Easing functions
EASEDEF double EaseQuadIn(double t, double b, double c, double d) { t /= d; return (c*t*t + b); }                         // Ease: Quadratic In
EASEDEF double EaseQuadOut(double t, double b, double c, double d) { t /= d; return (-c*t*(t - 2.0f) + b); }              // Ease: Quadratic Out
EASEDEF double EaseQuadInOut(double t, double b, double c, double d)                                                      // Ease: Quadratic In Out
{
    if ((t/=d/2) < 1) return (((c/2)*(t*t)) + b);
    return (-c/2.0f*(((t - 1.0f)*(t - 3.0f)) - 1.0f) + b);
}

// Exponential Easing functions
EASEDEF double EaseExpoIn(double t, double b, double c, double d) { return (t == 0.0f) ? b : (c*powf(2.0f, 10.0f*(t/d - 1.0f)) + b); }       // Ease: Exponential In
EASEDEF double EaseExpoOut(double t, double b, double c, double d) { return (t == d) ? (b + c) : (c*(-powf(2.0f, -10.0f*t/d) + 1.0f) + b); } // Ease: Exponential Out
EASEDEF double EaseExpoInOut(double t, double b, double c, double d)                                                                         // Ease: Exponential In Out
{
    if (t == 0.0f) return b;
    if (t == d) return (b + c);
    if ((t/=d/2.0f) < 1.0f) return (c/2.0f*powf(2.0f, 10.0f*(t - 1.0f)) + b);

    return (c/2.0f*(-powf(2.0f, -10.0f*(t - 1.0f)) + 2.0f) + b);
}

// Back Easing functions
EASEDEF double EaseBackIn(double t, double b, double c, double d) // Ease: Back In
{
    double s = 1.70158f;
    double postFix = t/=d;
    return (c*(postFix)*t*((s + 1.0f)*t - s) + b);
}

EASEDEF double EaseBackOut(double t, double b, double c, double d) // Ease: Back Out
{
    double s = 1.70158f;
    t = t/d - 1.0f;
    return (c*(t*t*((s + 1.0f)*t + s) + 1.0f) + b);
}

EASEDEF double EaseBackInOut(double t, double b, double c, double d) // Ease: Back In Out
{
    double s = 1.70158f;
    if ((t/=d/2.0f) < 1.0f)
    {
        s *= 1.525f;
        return (c/2.0f*(t*t*((s + 1.0f)*t - s)) + b);
    }

    double postFix = t-=2.0f;
    s *= 1.525f;
    return (c/2.0f*((postFix)*t*((s + 1.0f)*t + s) + 2.0f) + b);
}

// Bounce Easing functions
EASEDEF double EaseBounceOut(double t, double b, double c, double d) // Ease: Bounce Out
{
    if ((t/=d) < (1.0f/2.75f))
    {
        return (c*(7.5625f*t*t) + b);
    }
    else if (t < (2.0f/2.75f))
    {
        double postFix = t-=(1.5f/2.75f);
        return (c*(7.5625f*(postFix)*t + 0.75f) + b);
    }
    else if (t < (2.5/2.75))
    {
        double postFix = t-=(2.25f/2.75f);
        return (c*(7.5625f*(postFix)*t + 0.9375f) + b);
    }
    else
    {
        double postFix = t-=(2.625f/2.75f);
        return (c*(7.5625f*(postFix)*t + 0.984375f) + b);
    }
}

EASEDEF double EaseBounceIn(double t, double b, double c, double d) { return (c - EaseBounceOut(d - t, 0.0f, c, d) + b); } // Ease: Bounce In
EASEDEF double EaseBounceInOut(double t, double b, double c, double d) // Ease: Bounce In Out
{
    if (t < d/2.0f) return (EaseBounceIn(t*2.0f, 0.0f, c, d)*0.5f + b);
    else return (EaseBounceOut(t*2.0f - d, 0.0f, c, d)*0.5f + c*0.5f + b);
}

// Elastic Easing functions
EASEDEF double EaseElasticIn(double t, double b, double c, double d) // Ease: Elastic In
{
    if (t == 0.0f) return b;
    if ((t/=d) == 1.0f) return (b + c);

    double p = d*0.3f;
    double a = c;
    double s = p/4.0f;
    double postFix = a*powf(2.0f, 10.0f*(t-=1.0f));

    return (-(postFix*sinf((t*d-s)*(2.0f*PI)/p )) + b);
}

EASEDEF double EaseElasticOut(double t, double b, double c, double d) // Ease: Elastic Out
{
    if (t == 0.0f) return b;
    if ((t/=d) == 1.0f) return (b + c);

    double p = d*0.3f;
    double a = c;
    double s = p/4.0f;

    return (a*powf(2.0f,-10.0f*t)*sinf((t*d-s)*(2.0f*PI)/p) + c + b);
}

EASEDEF double EaseElasticInOut(double t, double b, double c, double d) // Ease: Elastic In Out
{
    if (t == 0.0f) return b;
    if ((t/=d/2.0f) == 2.0f) return (b + c);

    double p = d*(0.3f*1.5f);
    double a = c;
    double s = p/4.0f;

    if (t < 1.0f)
    {
        double postFix = a*powf(2.0f, 10.0f*(t-=1.0f));
        return -0.5f*(postFix*sinf((t*d-s)*(2.0f*PI)/p)) + b;
    }

    double postFix = a*powf(2.0f, -10.0f*(t-=1.0f));

    return (postFix*sinf((t*d-s)*(2.0f*PI)/p)*0.5f + c + b);
}

#if defined(__cplusplus)
}
#endif

#endif // REASINGS_H

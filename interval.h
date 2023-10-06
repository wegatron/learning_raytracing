#pragma once

#include "rtweekend.h"

class interval {
  public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    interval(double _min, double _max) : min(_min), max(_max) {}

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if(x < min) return min;
        else if(x > max) return max;
        return x;
    }

    interval expand(double delta)
    {
        return interval(min - 0.5 * delta, max + 0.5 * delta);
    }    

    double size()
    {
        return max - min;
    }

    static const interval empty, universe;
};

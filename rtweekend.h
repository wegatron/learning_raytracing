#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline int random_int(int min, int max)
{
    return rand()%(max-min) + min;
}
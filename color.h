#pragma once

#include "vec3.h"
#include "interval.h"
#include <iostream>

using color = vec3;

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
    pixel_color = pixel_color / samples_per_pixel;

    static const interval intensity(0, 0.99999);
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * intensity.clamp(pixel_color.x())) << ' '
        << static_cast<int>(255.999 * intensity.clamp(pixel_color.y())) << ' '
        << static_cast<int>(255.999 * intensity.clamp(pixel_color.z())) << '\n';
}
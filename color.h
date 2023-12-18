#pragma once

#include "vec3.h"
#include "interval.h"
#include <cassert>
#include <iostream>

using color = vec3;

vec3 linear2gamma(vec3 in)
{
    // rgb2srgb: https://lettier.github.io/3d-game-shaders-for-beginners/gamma-correction.html
    vec3 out(sqrt(in[0]), sqrt(in[1]), sqrt(in[2]));
    return out;

    // pow(in, 1/2.2)
}

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {

  assert(!std::isnan(pixel_color.x()) && !std::isnan(pixel_color.y()) &&
         !std::isnan(pixel_color.z()));

  pixel_color = pixel_color / samples_per_pixel;
  pixel_color = linear2gamma(pixel_color);
  static const interval intensity(0, 0.99999);
  // Write the translated [0,255] value of each color component.
  out << static_cast<int>(255.999 * intensity.clamp(pixel_color.x())) << ' '
      << static_cast<int>(255.999 * intensity.clamp(pixel_color.y())) << ' '
      << static_cast<int>(255.999 * intensity.clamp(pixel_color.z())) << '\n';
}
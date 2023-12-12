#pragma once

#include "rtweekend.h"
#include "vec3.h"

class onb {
public:

  onb() = default;
  
  void build_from_w(const vec3 &w)
  {
    axis[2] = unit_vector(w);
    axis[1] = fabs(w.z()) < 0.57 ? unit_vector(vec3(w.y(), -w.x(), 0)) : unit_vector(vec3(0, w.z(), -w.y()));
    axis[0] = cross(axis[1], axis[2]);
  }

    vec3 local(double a, double b, double c) const
    {
        return a * axis[0] + b * axis[1] + c * axis[2];
    }

    vec3 local(const vec3 &a) const
    {
        return a.x() * axis[0] + a.y() * axis[1] + a.z() * axis[2];
    }

    vec3 w() const { return axis[2]; }

private:
    vec3 axis[3];
};
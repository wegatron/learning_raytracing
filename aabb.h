#pragma once

#include "interval.h"
#include "ray.h"

class aabb {
public:
  interval x, y, z;

  aabb(const point3 &a, const point3 &b)
  {
    x.min = a.x();
    x.max = b.x();
    if(x.min > x.max) std::swap(x.min, x.max);

    y.min = a.y();
    y.max = b.y();
    if(y.min > y.max) std::swap(y.min, y.max);

    z.min = a.z();
    z.max = b.z();
    if(z.min > z.max) std::swap(z.min, z.max);
  }

  aabb() {}

  aabb(const interval &ix, const interval &iy, const interval &iz)
      : x(ix), y(iy), z(iz) {}

  aabb(const aabb &a, const aabb &b) {
    x.min = std::min(b.x.min, a.x.min);
    y.min = std::min(b.y.min, a.y.min);
    z.min = std::min(b.z.min, a.z.min);

    x.max = std::max(b.x.max, a.x.max);
    y.max = std::max(b.y.max, a.y.max);
    z.max = std::max(b.z.max, a.z.max);
  }
  
  void set(const interval &ix, const interval &iy, const interval &iz) {
    x = ix;
    y = iy;
    z = iz;
  }

  interval operator[](int index) {
    if (index == 0)
      return x;
    if (index == 1)
      return y;
    return z;
  }

  aabb operator+(const vec3 &t) const {
    aabb ret = *this;
    ret.x.min += t[0];
    ret.x.max += t[0];
    ret.y.min += t[1];
    ret.y.max += t[1];
    ret.z.min += t[2];
    ret.z.max += t[2];
    return ret;
  }

  const interval& axis(int n) const {
      if (n == 1) return y;
      if (n == 2) return z;
      return x;
  }  

  aabb pad()
  {
      // Return an AABB that has no side narrower than some delta, padding if necessary.
        double delta = 0.0001;
        interval new_x = (x.size() >= delta) ? x : x.expand(delta);
        interval new_y = (y.size() >= delta) ? y : y.expand(delta);
        interval new_z = (z.size() >= delta) ? z : z.expand(delta);

        return aabb(new_x, new_y, new_z);    
  }

  void merge(const aabb &b) {
    x.min = std::min(b.x.min, x.min);
    y.min = std::min(b.y.min, y.min);
    z.min = std::min(b.z.min, z.min);

    x.max = std::max(b.x.max, x.max);
    y.max = std::max(b.y.max, y.max);
    z.max = std::max(b.z.max, z.max);
  }

  bool hit(const ray& r, interval ray_t) const {
    for (int a = 0; a < 3; a++) {
        auto invD = 1 / r.direction()[a];
        auto orig = r.origin()[a];

        auto t0 = (axis(a).min - orig) * invD;
        auto t1 = (axis(a).max - orig) * invD;

        if (invD < 0)
            std::swap(t0, t1);

        if (t0 > ray_t.min) ray_t.min = t0;
        if (t1 < ray_t.max) ray_t.max = t1;

        if (ray_t.max <= ray_t.min)
            return false;
    }
      return true;
  }
};
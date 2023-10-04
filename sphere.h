#pragma once

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"
#include "material.h"

class sphere : public hittable {
public:
  sphere(point3 _center, double _radius, const std::shared_ptr<material> &m)
      : center(_center), radius(_radius),
      bbox(
        interval(center.x()-radius, center.x()+radius),
        interval(center.y()-radius, center.y()+radius),
        interval(center.z()-radius, center.z()+radius)),
      mat(m) {
      }

  sphere(point3 _center, double _radius, const std::shared_ptr<material> &m,
         const vec3 &_speed)
      : center(_center), radius(_radius), speed(_speed), mat(m) {
        interval ix, iy, iz;

        if(_speed.x() > 0)
          ix = interval(center.x()-radius, center.x()+radius+_speed.x());        
        else ix = interval(center.x()-radius+_speed.x(), center.x()+radius);

        if(_speed.y() > 0)
          iy = interval(center.y()-radius, center.y()+radius+_speed.y());        
        else iy = interval(center.y()-radius+_speed.y(), center.y()+radius);

        if(_speed.z() > 0)
          iz = interval(center.z()-radius, center.z()+radius+_speed.z());        
        else iz = interval(center.z()-radius+_speed.z(), center.z()+radius);
      }

  bool hit(const ray &r, interval ray_t,
           hit_record &rec) const override {
    point3 cur_center = center + r.time() * speed;
    vec3 oc = r.origin() - cur_center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
      return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    
    if (!ray_t.contains(root)) {
      root = (-half_b + sqrtd) / a;
      if (!ray_t.contains(root))
        return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - cur_center) / radius;

    vec3 outward_normal = (rec.p - cur_center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat = mat;
    return true;
  }

  aabb bounding_box() const override
  {
    return bbox;
  }

private:
  point3 center;
  double radius;
  vec3 speed{0, 0, 0};
  aabb bbox;
  std::shared_ptr<material> mat;
};

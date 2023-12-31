#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"
#include <memory>

class constant_medium : public hittable {
public:
  constant_medium(std::shared_ptr<hittable> b, double d,
                  std::shared_ptr<texture> a)
      : boundary(b),
        phase_function(std::make_shared<isotropic>(a)),
        neg_inv_density(-1 / d) {}

  constant_medium(std::shared_ptr<hittable> b, double d, color c)
      : boundary(b), 
        phase_function(std::make_shared<isotropic>(c)),
        neg_inv_density(-1 / d) {}

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    // todo
    hit_record rec1, rec2;
    if (!boundary->hit(r, ray_t, rec1))
      return false;
    if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
      return false;

    auto ray_length = r.direction().length();
    auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    auto hit_distance = neg_inv_density * log(random_double());
    if (hit_distance > distance_inside_boundary)
      return false;
    
    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);
    rec.normal = vec3(1, 0, 0); // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat = phase_function;

    return true;
  }

  aabb bounding_box() const override { return boundary->bounding_box(); }

private:
  std::shared_ptr<hittable> boundary;
  std::shared_ptr<material> phase_function;
  double neg_inv_density;
};
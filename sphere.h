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

  void get_sphere_uv(const point3 &p, double &u, double &v) const
  {
    v = acos(-p.y())/pi;
    u = (atan2(-p.z(), p.x()) + pi)/(2*pi);
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
    vec3 outward_normal = (rec.p - cur_center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);

    rec.mat = mat;
    return true;
  }

  aabb bounding_box() const override
  {
    return bbox;
  }

  double pdf_value(const point3& origin, const vec3 &v) const override
  {
    hit_record rec;
    if(!this->hit(ray(origin, v), interval::empty, rec))
      return 0.0;

    auto cos_theta_max = sqrt(1 - radius*radius/(center-origin).length_squared());
    auto solid_angle = 2*pi*(1-cos_theta_max);

    return 1.0/solid_angle;
  }

  vec3 random(const vec3 &origin) const override
  {
    vec3 direction = center - origin;    
    double phi = 2*pi*random_double();    
    auto cos_theta_max = sqrt(1 - radius*radius/(center-origin).length_squared());
    double r = random_double();
    
    double z = 1 + r*(cos_theta_max-1); // cos_theta
    auto sin_theta = sqrt(1-z*z);
    double x = cos(phi)*sin_theta;
    double y = sin(phi)*sin_theta;
    
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(x, y, z);
  }

private:
  point3 center;
  double radius;
  vec3 speed{0, 0, 0};
  aabb bbox;
  std::shared_ptr<material> mat;
};

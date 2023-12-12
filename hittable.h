#pragma once

#include "ray.h"
#include "interval.h"
#include "aabb.h"
#include <memory>
#include <algorithm>

class material;

class hit_record {
  public:
    double u, v;
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    std::shared_ptr<material> mat;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }    
};

class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    virtual aabb bounding_box() const = 0;

    virtual double pdf_value(const point3& origin, const vec3 &v) const
    {
      return 0.0;    
    }

    virtual vec3 random(const vec3 &origin) const
    {
      return vec3(1,0,0);
    }
};

class rotate_y : public hittable {
  public:
    rotate_y(std::shared_ptr<hittable> p, double angle): ptr(p), sin_theta(sin(angle)), cos_theta(cos(angle))
    {
      bbox = p->bounding_box();
      double x[4] = {bbox.x.min, bbox.x.min, bbox.x.max, bbox.x.max};
      double z[4] = {bbox.z.min, bbox.z.max, bbox.z.min, bbox.z.max};

      double max_x = 0, min_x = 0;
      double max_z = 0, min_z = 0;
      max_x = min_x = cos_theta * x[0] + sin_theta * z[0];
      max_z = min_z = -sin_theta * x[0] + cos_theta * z[0];

      max_x = std::max(max_x, cos_theta * x[1] + sin_theta * z[1]);
      min_x = std::min(min_x, cos_theta * x[1] + sin_theta * z[1]);
      max_z = std::max(max_z, -sin_theta * x[1] + cos_theta * z[1]);
      min_z = std::min(min_z, -sin_theta * x[1] + cos_theta * z[1]);

      max_x = std::max(max_x, cos_theta * x[2] + sin_theta * z[2]);
      min_x = std::min(min_x, cos_theta * x[2] + sin_theta * z[2]);
      max_z = std::max(max_z, -sin_theta * x[2] + cos_theta * z[2]);
      min_z = std::min(min_z, -sin_theta * x[2] + cos_theta * z[2]);

      max_x = std::max(max_x, cos_theta * x[3] + sin_theta * z[3]);
      min_x = std::min(min_x, cos_theta * x[3] + sin_theta * z[3]);
      max_z = std::max(max_z, -sin_theta * x[3] + cos_theta * z[3]);
      min_z = std::min(min_z, -sin_theta * x[3] + cos_theta * z[3]);

      bbox.x.min = min_x;
      bbox.x.max = max_x; 

      bbox.z.min = min_z;
      bbox.z.max = max_z;     
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        // Change the ray from world space to object space
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
        origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

        direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
        direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

        ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where)
        if (!ptr->hit(rotated_r, ray_t, rec))
            return false;

        // Change the intersection point from object space to world space
        auto p = rec.p;
        p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

        // Change the normal from object space to world space
        auto normal = rec.normal;
        normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;      
    }

    aabb bounding_box() const override {
        return bbox;
    }

  public:
    std::shared_ptr<hittable> ptr;
    float sin_theta;
    float cos_theta;
    bool hasbox;
    aabb bbox;
};


class translate : public hittable {
  public:
    translate(std::shared_ptr<hittable> p, const vec3& displacement)
      : object(p), offset(displacement)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Move the ray backwards by the offset
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p += offset;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

  private:
    std::shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};
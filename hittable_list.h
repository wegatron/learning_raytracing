#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        bbox.merge(object->bounding_box());
        objects.push_back(object);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        
        for (const auto& object : objects) {
            if (object->hit(r, ray_t, temp_rec)) {
                hit_anything = true;
                ray_t.max = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    double pdf_value(const point3& origin, const vec3 &v) const override
    {
      auto weight = 1.0/objects.size();
      auto sum = 0.0;

      for(const auto& object : objects)
        sum += weight * object->pdf_value(origin, v);

      return sum;
    }

    vec3 random(const vec3 &origin) const override {
        if (objects.empty()) {
            return vec3(0,0,0);
        }
        return objects[random_int(0, objects.size()-1)]->random(origin);
    }

private:
    aabb bbox{interval::empty, interval::empty, interval::empty};
};

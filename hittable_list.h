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

private:
    aabb bbox{interval::empty, interval::empty, interval::empty};
};

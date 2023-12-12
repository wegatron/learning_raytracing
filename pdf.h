#pragma once

#include "rtweekend.h"
#include "vec3.h"
#include "onb.h"
#include "hittable.h"

class pdf {
public:
  virtual ~pdf() = default;

  /**
   * \brief returning the corresponding PDF distribution value in that direction.
  */
  virtual double value(const vec3 &direction) const = 0;

  /**
   * \brief returning a random direction weighted by the internal PDF distribution
  */
  virtual vec3 generate() const = 0;
};

class sphere_pdf : public pdf {
public:
  sphere_pdf() = default;

  double value(const vec3 &direction) const override { return 1 / (4 * pi); }

  vec3 generate() const override {
    auto r1 = random_double();
    auto r2 = random_double();
    double phi = 2 * pi * r1;
    double theta = acos(1 - 2 * r2);
    return vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  }
};

class hemisphere_pdf : public pdf {
public:
  hemisphere_pdf() = default;

  double value(const vec3 &direction) const override { return 1 / (2 * pi); }

  vec3 generate() const override {
    auto r1 = random_double();
    auto r2 = random_double();
    double phi = 2 * pi * r1;
    double theta = acos(1 - r2);
    return vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
  }
};

class cosine_pdf : public pdf {
  public:
    cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

    double value(const vec3& direction) const override {
        auto cosine_theta = dot(unit_vector(direction), uvw.w());
        return fmax(0, cosine_theta/pi);
    }

    vec3 generate() const override {
      auto r1 = random_double();
      auto r2 = random_double();
      double phi = 2 * pi * r1;
      return uvw.local(cos(phi)*sqrt(r2), sin(phi)*sqrt(r2), sqrt(1-r2));
    }

  private:
    onb uvw;
};

class hittable_pdf : public pdf {
  public:
    hittable_pdf(const hittable &o, const vec3 &origin) : obj(&o), origin(origin) {}

    double value(const vec3 &direction) const override {
      return obj->pdf_value(origin, direction);
    }

    vec3 generate() const override {
      return obj->random(origin);
    }
  private:
    const hittable * obj;
    point3 origin;
};

class mixture_pdf : public pdf{
  public:
    mixture_pdf(const std::initializer_list<std::shared_ptr<pdf>> &pl, const std::initializer_list<double> &ws) : pdf_list(pl), weights(ws) {}
    
    double value(const vec3 &direction) const override {
      double sum = 0;
      for (size_t i = 0; i < pdf_list.size(); ++i) {
        sum += weights[i] * pdf_list[i]->value(direction);
      }
      return sum;
    }

    vec3 generate() const override {
      auto r = random_double();
      double sum = 0;
      for (size_t i = 0; i < pdf_list.size(); ++i) {
        sum += weights[i];
        if (r < sum) {
          return pdf_list[i]->generate();
        }
      }
      return pdf_list.back()->generate();
    }
  private:
    std::vector<std::shared_ptr<pdf>> pdf_list;
    std::vector<double> weights;
};

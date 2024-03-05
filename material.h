#pragma once

#include <algorithm>
#include "rtweekend.h"
#include "color.h"
#include "texture.h"
#include "pdf.h"

class hit_record;
class ray;

class material
{
public:
    virtual ~material() = default;
    virtual color emitted(double u, double v, const point3 &p) const
    {
        return color(0,0,0);
    }
    
    virtual bool scatter(const ray &in, const std::shared_ptr<pdf> &sample_pdf, const hit_record &rec, color &attenuation, ray &scattered) const = 0;

    virtual double scattering_pdf(const ray &r_in, const hit_record &rec,
                          const ray &scattered)
    const {
        return 0;    
    }
};

class lambertian : public material
{
public:
    lambertian(const std::shared_ptr<texture> &a) : albedo(a) {}
    lambertian(const color &a) : albedo(std::make_shared<solid_color>(a)) {}
    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered)
    const override {
        auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta/pi;
    }    
    bool scatter(const ray &in, const std::shared_ptr<pdf> &sample_pdf, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();
        if(scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction, in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);

        // auto scatter_direction = sample_pdf->generate();
        // double pdf_value = sample_pdf->value(scatter_direction);

        // // if(pdf_value <= 0.0)
        // // {
        // //     std::cout << "negative attenuation" << std::endl;
        // //     auto aa = sample_pdf->value(scatter_direction);
        // // }

        
        // scattered = ray(rec.p, scatter_direction, in.time());
        // double spdf = scattering_pdf(in, rec, scattered);
        // attenuation = albedo->value(rec.u, rec.v, rec.p) * spdf/pdf_value;
            
        // assert(attenuation.x() >= 0.0 && attenuation.y() >= 0.0 &&
        //        attenuation.z() >= 0.0);
        // assert(attenuation.x() < 1.0 && attenuation.y() < 1.0 &&
        //        attenuation.z() < 1.0);
        return true;
    }

private:
    std::shared_ptr<texture> albedo;
};

class metal : public material
{
public:
    metal(const color &a, const double f) : albedo(a), fuzz(std::clamp(f, 0.0, 1.0)) {}
    bool scatter(const ray &in, const std::shared_ptr<pdf> &sample_pdf, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        auto reflect_direction = reflect(unit_vector(in.direction()), rec.normal);
        reflect_direction = unit_vector(reflect_direction) + fuzz * random_unit_vector();
        attenuation = albedo;
        scattered = ray(rec.p, reflect_direction, in.time());
        return dot(scattered.direction(), rec.normal) > 0;
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    dielectric(const double ri) : reflection_index(ri) {}
    bool scatter(const ray &in, const std::shared_ptr<pdf> &sample_pdf, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        vec3 uin_dir = unit_vector(in.direction());
        double ir = rec.front_face ? 1.0/reflection_index : reflection_index;

        // judge recract or reflect
        double ct = fabs(dot(uin_dir, rec.normal));
        double st = sqrt(1-ct*ct);
        bool cannot_recract = st * ir > 1;
        if(cannot_recract || random_double() < reflectance(ct, ir))
        {
            vec3 dir = reflect(uin_dir, rec.normal);
            scattered = ray(rec.p, dir, in.time());
            return true;
        }
        vec3 dir = refract(uin_dir, rec.normal, ir);
        scattered = ray(rec.p, dir, in.time());
        return true;
    }
private:
    double reflection_index{1.0};

    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }    
};

class diffuse_light : public material
{
public:
    diffuse_light(std::shared_ptr<texture> &a) : albedo(a) {}

    diffuse_light(const color &a) : albedo(std::make_shared<solid_color>(a)) {}

    color emitted(double u, double v, const point3 &p) const override
    {
        return albedo->value(u, v, p);
    }
    bool scatter(const ray &in, const std::shared_ptr<pdf> &sample_pdf, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        return false;
    }
private:
    std::shared_ptr<texture> albedo;
};

class isotropic : public material {
  public:
    isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    bool scatter(const ray& r_in, const std::shared_ptr<pdf> &sample_pdf, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

  private:
    shared_ptr<texture> albedo;
};
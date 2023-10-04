#pragma once

#include "rtweekend.h"
#include "color.h"
#include <algorithm>

class hit_record;
class ray;

class material
{
public:
    virtual ~material() = default;
    virtual bool scatter(const ray &in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
};

class lambertian : public material
{
public:
    lambertian(const color &a) : albedo(a) {}
    bool scatter(const ray &in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {        
        auto scatter_direction = rec.normal + random_unit_vector();
        if(scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction, in.time());
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

class metal : public material
{
public:
    metal(const color &a, const double f) : albedo(a), fuzz(std::clamp(f, 0.0, 1.0)) {}
    bool scatter(const ray &in, const hit_record &rec, color &attenuation, ray &scattered) const override
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
    bool scatter(const ray &in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        vec3 uin_dir = unit_vector(in.direction());
        double ir = rec.front_face ? 1.0/reflection_index : reflection_index;
        // judge recract or reflect
        double ct = dot(uin_dir, rec.normal);
        double st = sqrt(1-ct*ct);
        bool can_recract = st * ir < 1;
        vec3 dir = can_recract ? refract(uin_dir, rec.normal, ir) : reflect(uin_dir, rec.normal);        
        scattered = ray(rec.p, dir, in.time());
        return true;
    }
private:
    double reflection_index{1.0};
};
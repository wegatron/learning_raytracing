#pragma once

#include <fstream>
#include <vector>

#include "color.h"
#include "hittable.h"
#include "interval.h"
#include "pdf.h"
#include "ray.h"
#include "rtweekend.h"

class camera {
public:
  /* Public Camera Parameters Here */
  double aspect_ratio = 1.0; // Ratio of image width over height
  int image_width = 100;     // Rendered image width in pixel count
  int samples_per_pixel = 10;
  int max_depth = 50;

  double focus_dist{1.0};
  double defocus_angle{-1};

  double vfov{90};
  point3 center{0, 0, 0}; // Camera center
  double shutter_time{0};
  color background{0, 0, 0};

  void lookat(const point3 &pt, const vec3 &_up) {
    look_dir = unit_vector(pt - center);
    up = unit_vector(_up - look_dir * dot(look_dir, _up));
  }
  void render(const hittable &world, const hittable * lights) {
    initialize();
    std::ofstream ofs("output/image.ppm");
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
      std::clog << "\rScanlines remaining: " << (image_height - j) << ' '
                << std::flush;
      for (int i = 0; i < image_width; ++i) {

        color pixel_color(0, 0, 0);
        for (auto sample = 0; sample < samples_per_pixel; ++sample) {
          auto ru = random_double();
          auto rv = random_double();
          auto pixel_center = pixel00_loc + (i + ru - 0.5) * pixel_delta_u +
                              (j - 0.5 + rv) * pixel_delta_v;

          auto ray_origin =
              (defocus_angle <= 0) ? center : defocus_disk_sample();
          auto ray_direction = pixel_center - ray_origin;

          double delta_time = random_double() * shutter_time;
          ray r(ray_origin, ray_direction, delta_time);

          pixel_color += ray_color(r, max_depth, world, lights);
        }

        write_color(ofs, pixel_color, samples_per_pixel);
      }
    }
    ofs.close();
    std::clog << "\rDone.                 \n";
  }

private:
  vec3 look_dir{0, 0, -1};
  vec3 up = {0, 1, 0};

  vec3 defocus_disk_u; // Defocus disk horizontal radius
  vec3 defocus_disk_v; // Defocus disk vertical radius

  /* Private Camera Variables Here */
  int image_height;   // Rendered image height
  point3 pixel00_loc; // Location of pixel 0, 0
  vec3 pixel_delta_u; // Offset to pixel to the right
  vec3 pixel_delta_v; // Offset to pixel below

  void initialize() {
    image_height = image_width / aspect_ratio;
    image_height = (image_height < 1) ? 1 : image_height;
    auto viewport_height = 2 * tan(0.5 * vfov * pi / 180) * focus_dist;
    auto viewport_width =
        viewport_height * (static_cast<double>(image_width) / image_height);
    auto left = cross(up, look_dir);
    auto viewport_upper_left =
        center + focus_dist * look_dir +
        0.5 * (up * viewport_height + left * viewport_width);
    pixel_delta_u = -left * (viewport_width / image_width);
    pixel_delta_v = -up * (viewport_height / image_height);
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius = focus_dist * tan(pi * defocus_angle / 360);
    defocus_disk_u = -defocus_radius * left;
    defocus_disk_v = -defocus_radius * up;
  }

  point3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  color ray_color(const ray &r, int depth, const hittable &world,
                  const hittable * lights) const {
    if (--depth == 0) {
      return color(0, 0, 0);
    }

    hit_record rec;
    if (world.hit(r, interval(0.001, infinity), rec)) {
      ray scattered;
      color attenuation;
      color emitted = rec.mat->emitted(rec.u, rec.v, rec.p);

      // sample pdf
      std::initializer_list<double> weights = {0.5, 0.5};
      std::initializer_list<std::shared_ptr<pdf>> pdfs;
      if (lights != nullptr) {
        pdfs = {
            std::make_shared<cosine_pdf>(rec.normal),
            std::make_shared<hittable_pdf>(*lights, rec.p)};
      } else {
        pdfs = {
            std::make_shared<cosine_pdf>(rec.normal)};
      }

      auto p_mix = std::make_shared<mixture_pdf>(pdfs, weights);
      if (rec.mat->scatter(r, p_mix, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, depth, world, lights) + emitted;
      return emitted;
    }

    return background;
    // background color
    // if(background != nullptr) return background
    // vec3 unit_direction = unit_vector(r.direction());
    // auto a = 0.5 * (unit_direction.y() + 1.0);
    // return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  }
};
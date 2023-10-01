#pragma once

#include "hittable.h"
#include "ray.h"
#include "color.h"
#include "rtweekend.h"


class camera {
  public:
    /* Public Camera Parameters Here */
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int samples_per_pixel = 10;
    int max_depth = 10;

    double vfov{90};
    point3 center{0, 0, 0};         // Camera center
    vec3 look_dir{0, 0, -1};
    vec3 up = {0, 1, 0};

    void render(const hittable& world) {
        initialize();
        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                
                color pixel_color(0,0,0);
                for(auto sample=0; sample < samples_per_pixel; ++sample)
                {
                    auto ru = random_double();
                    auto rv = random_double();
                    auto pixel_center = pixel00_loc + (i+ru-0.5) * pixel_delta_u + (j-0.5+rv) * pixel_delta_v;
                    auto ray_direction = pixel_center - center;
                    ray r(center, ray_direction);

                    pixel_color += ray_color(r, max_depth, world);
                }

                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }

  private:
    /* Private Camera Variables Here */
    int    image_height;   // Rendered image height
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below

    void initialize() {
        image_height = image_width / aspect_ratio;
        image_height = (image_height < 1) ? 1 : image_height;
        auto focal_length = 1.0;
        auto viewport_height = 2 * tan(0.5*vfov*pi/180) * focal_length;
        auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);
        auto left = cross(up, look_dir);
        auto viewport_upper_left =  center + focal_length * look_dir + 0.5 * (up * viewport_height + left * viewport_width);
        pixel_delta_u = -left * (viewport_width / image_width);
        pixel_delta_v = -up * (viewport_height / image_height);
        pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        if(--depth == 0)
        {
            return color(0,0,0);
        }

        hit_record rec;
        if(world.hit(r, 0.001, infinity, rec))
        {
            ray scattered;
            color attenuation;
            if(rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth, world);
            return color(0.0, 0.0, 0.0);
        }
        
        // background color
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);        
    }
};
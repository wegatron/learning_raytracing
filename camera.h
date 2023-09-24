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
                    auto pixel_center = pixel00_loc + vec3((i+ru-0.5) * pixel_delta_u, (j-0.5+rv) * pixel_delta_v, 0);
                    auto ray_direction = pixel_center - center;
                    ray r(center, ray_direction);

                    pixel_color += ray_color(r, world);
                }

                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";        
    }

  private:
    /* Private Camera Variables Here */
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    double   pixel_delta_u;  // Offset to pixel to the right
    double   pixel_delta_v;  // Offset to pixel below

    void initialize() {
        image_height = image_width / aspect_ratio;
        image_height = (image_height < 1) ? 1 : image_height;
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);
        center = point3(0, 0, 0);        
        auto viewport_upper_left = center - 0.5*vec3(viewport_width, -viewport_height, 2*focal_length);
        pixel_delta_u = viewport_width / image_width;
        pixel_delta_v = -viewport_height / image_height;        
        pixel00_loc = viewport_upper_left + 0.5*vec3(pixel_delta_u, pixel_delta_v, 0);
    }

    color ray_color(const ray& r, const hittable& world) const {
        hit_record rec;
        if(world.hit(r, 0, infinity, rec))
        {
            vec3 direction = random_on_hemisphere(rec.normal);
            return 0.5 * ray_color(ray(rec.p, direction), world);
            //return 0.5*(rec.normal + color(1,1,1));
        }
        
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);        
    }
};
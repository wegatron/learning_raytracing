#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

#include <iostream>


int main() {
    // World

    hittable_list world;

    auto mat_ground = std::make_shared<lambertian>(color(0.4, 0.5, 0.6));
    auto mat_center = std::make_shared<lambertian>(color(0.6, 0.5, 0.1));
    // auto mat_left = std::make_shared<metal>(color(0.6, 0.2, 0.6), 0.7);
    // auto mat_right = std::make_shared<metal>(color(0.1, 0.3, 0.4), 0.1);

    auto mat_left = std::make_shared<dielectric>(1.2);
    auto mat_right = std::make_shared<dielectric>(1.6);    

    world.add(make_shared<sphere>(point3(0,0,-1), 0.5, mat_center));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100, mat_ground));
    world.add(make_shared<sphere>(point3(-1,0,-1), 0.5, mat_left));
    world.add(make_shared<sphere>(point3(1,0,-1), 0.5, mat_right));

    camera cam;
    cam.samples_per_pixel = 10;
    cam.image_width = 800;
    cam.aspect_ratio = 16.0/9.0;
    cam.render(world);
}
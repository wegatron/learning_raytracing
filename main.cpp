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

    auto mat_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto mat_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto mat_left   = std::make_shared<dielectric>(1.5);
    auto mat_right  = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);  

    world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, mat_ground));
    world.add(std::make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, mat_center));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, mat_left));
    world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, mat_left));
    world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, mat_right));

    camera cam;
    cam.vfov = 60;
    cam.defocus_angle = 4.0;
    cam.focus_dist    = 3.4;    
    cam.center = vec3(-2, 2, 1);
    cam.lookat(point3(0,0,0), vec3(0,1,0));
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.image_width = 400;
    cam.aspect_ratio = 16.0/9.0;
    cam.render(world);
}
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"

#include <iostream>

using namespace std;

const interval interval::empty   (+infinity, -infinity);
const interval interval::universe(-infinity, +infinity);

int main()
{
    hittable_list world;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    vec3 speed{0, random_double()*40, 0};
                    world.add(make_shared<sphere>(center, 0.2, sphere_material, speed));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    auto world2 = std::make_shared<bvh_node>(world);

    camera cam;

    cam.shutter_time = 0.01;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.center = point3(13,2,3);
    cam.lookat(point3(0,0,0), vec3(0,1,0));
    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(*world2);
    return 0;
}


// int main() {
//     // World

//     hittable_list world;

//     auto mat_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
//     auto mat_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
//     auto mat_left   = std::make_shared<dielectric>(1.5);
//     auto mat_right  = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);  

//     world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, mat_ground));
//     world.add(std::make_shared<sphere>(point3( 0.0, 0.0, -1.0),   0.5, mat_center));
//     world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, mat_left));
//     world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, mat_left));
//     world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, mat_right));

//     camera cam;
//     cam.vfov = 20;
//     cam.defocus_angle = 10.0;
//     cam.focus_dist    = 3;    
//     cam.center = vec3(-2, 2, 1);
//     cam.lookat(point3(0,0,-1), vec3(0,1,0));
//     cam.samples_per_pixel = 100;
//     cam.max_depth = 50;
//     cam.image_width = 400;
//     cam.aspect_ratio = 16.0/9.0;
//     cam.render(world);
// }
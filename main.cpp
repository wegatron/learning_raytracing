#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "constant_medium.h"
#include <chrono>
#include <iostream>

using namespace std;

const interval interval::empty   (+infinity, -infinity);
const interval interval::universe(-infinity, +infinity);

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.center = point3(0,0, 12);
    cam.lookat(point3(0,0,0), vec3(0,1,0));

    cam.defocus_angle = 0;

    cam.render(hittable_list(globe), nullptr);
}

void many_balls()
{
    hittable_list world;
    auto checker = make_shared<checker_texture> (0.32, color(.2, .3, .1), color(.9, .9, .9));
    //auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto ground_material = make_shared<lambertian>(checker);
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

    cam.render(*world2, nullptr);    
}


void random_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

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

                    //auto center2 = center + vec3(0, random_double(0,.5), 0);
                    //world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));

                    //vec3 speed{0, random_double()*40, 0};
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
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

    auto world2 = make_shared<bvh_node>(world);

    camera cam;

    cam.background        = color(0.70, 0.80, 1.00);     
    
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

    cam.render(*world2, nullptr);
}

void simple_light()
{
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    //auto color_texture = make_shared<solid_color>(color(0.8, 0.8, 0.8));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    auto light = make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0),
                      difflight);
    world.add(light);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 20;
    cam.center = point3(26,3,6);
    cam.lookat(point3(0,2,0), vec3(0,1,0));

    cam.defocus_angle = 0;

    cam.render(world, light.get());
}

void cornell_box() {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto specular_white = make_shared<metal>(color(0.9, 0.9, 0.9), 0.0);
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));


    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15/180.0*pi);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    world.add(box1);

    // shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
    // box2 = make_shared<rotate_y>(box2, -18/180.0*pi);
    // box2 = make_shared<translate>(box2, vec3(130,0,65));    
    // world.add(box2);

    auto quad_light = make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105),
                      light);
    world.add(quad_light);    
    
    // Glass Sphere
    auto glass = make_shared<dielectric>(1.5);
    auto glass_sphere = make_shared<sphere>(point3(190, 90, 190), 90, glass);
    world.add(glass_sphere);

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.center = point3(278, 278, -800);
    cam.lookat(point3(278, 278, 0), vec3(0,1,0));

    cam.defocus_angle = 0;

    hittable_list sample_hittables;
    sample_hittables.add(quad_light);
    sample_hittables.add(glass_sphere);
    cam.render(world, &sample_hittables);
}


void cornell_smoke() {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));

    auto quad_light = make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0),
                                   vec3(0, 0, 305), light);
    world.add(quad_light);
    world.add(make_shared<quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15/180.0*pi);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18/180.0*pi);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    world.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.center = point3(278, 278, -800);
    cam.lookat(point3(278, 278, 0), vec3(0,1,0));
    cam.defocus_angle = 0;

    cam.render(world, quad_light.get());
}

int main()
{
    //earth();
    //cornell_box();
    //cornell_smoke();
    auto tp0 = std::chrono::high_resolution_clock::now();
    random_spheres();
    std::clog << "Time cost:"
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now() - tp0)
                   .count()
            << "ms" << std::endl;    
    return 0;
}
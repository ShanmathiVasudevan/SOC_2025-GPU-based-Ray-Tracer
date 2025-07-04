#ifndef CAMERA_H
#define CAMERA_H
#include "vec3.h"
#include "hittable.h"
#include "material.h"
class camera {
  public:
    double aspect_ratio = 16.0/9.0;  // Ratio of image width over height
    int    image_width  = 400;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int max_depth = 10;
    void render(hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color = pixel_color + ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_color*pixel_samples_scale);
            }
        }
        std::clog << "\rDone.                 \n";
    }

  private:
    int    image_height;   // Rendered image height
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    Vec3   pixel_delta_u;  // Offset to pixel to the right
    Vec3   pixel_delta_v;  // Offset to pixel below

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        pixel_samples_scale = 1.0 / samples_per_pixel;
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);
        center = point3(0, 0, 0);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        Vec3 viewport_u = Vec3(viewport_width, 0, 0);
        Vec3 viewport_v = Vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - Vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)*0.5;
    }

    ray get_ray(int i, int j) {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        Vec3 pixel_sample = pixel00_loc
                          + pixel_delta_u*(i + offset[0])
                          + pixel_delta_v*(j + offset[1]);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    Vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(ray& r, int depth, hittable& world) {
        if(depth <= 0){
            return color(0,0,0);
        }
        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            //Vec3 direction = rec.normal.random_on_hemisphere(); // simple diffuse
            /*Vec3 direction = rec.normal + rec.normal.random_unit_vector(); // lambertian diffuse
            auto x = ray(rec.p, direction);
            return ray_color(x,depth-1, world) * 0.5;*/
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)){
                auto x = ray_color(scattered, depth-1, world);
                auto y = Vec3(x[0]*attenuation[0],x[1]*attenuation[1],x[2]*attenuation[2]);
                return y;
            }
            return color(0,0,0);
        }
        Vec3 unit_direction = (r.direction()).unit();
        auto a = 0.5*(unit_direction[1] + 1.0);
        //std::clog << " Ray unit direction "<<unit_direction[0]<<unit_direction[1]<<unit_direction[2]<< " a value "<<a <<"\n";
        return color(1.0, 1.0, 1.0)*(1.0-a) + color(0.5, 0.7, 1.0)*a;
    }
};

#endif
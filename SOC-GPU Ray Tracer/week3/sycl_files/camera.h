#ifndef CAMERA_H
#define CAMERA_H
#include "rtweekend.h"
#include "sphere.h"

namespace constants {
  static constexpr auto TileX = 8;
  static constexpr auto TileY = 8;
}
class camera {
  public:
    float aspect_ratio = 4.0f / 3.0f;
    int image_width = 640;
    int image_height = (int(image_width / aspect_ratio) < 1) ? 1 : int(image_width / aspect_ratio); 
    float focal_length = 1.0f;
    float viewport_height = 2.0f;
    float viewport_width = viewport_height * (float(image_width)/image_height);
    Vec3 viewport_u = Vec3(viewport_width, 0.0f, 0.0f);
    Vec3 viewport_v = Vec3(0.0f, -viewport_height, 0.0f);
    Vec3 pixel_delta_u = viewport_u / image_width;
    Vec3 pixel_delta_v = viewport_v / image_height;
    point3 center = point3(0.0f,0.0f,0.0f);         // Camera center
    Vec3 viewport_upper_left = center - Vec3(0.0f, 0.0f, focal_length) - viewport_u/2.0f - viewport_v/2.0f;
    Vec3 pixel00_loc = viewport_upper_left + (pixel_delta_u + pixel_delta_v)*0.5f; 
    ray get_ray(float u, float v){
        return ray(center, pixel00_loc + pixel_delta_u*u + pixel_delta_v*v - center);
    }
};

template <int width, int height, int num_spheres> 
class render_kernel {
    public:
    render_kernel(sycl::accessor<Vec3, 1, sycl::access::mode::write, sycl::access::target::device> frame_ptr, sycl::accessor<sphere, 1, sycl::access::mode::read, sycl::access::target::device> spheres_ptr): m_frame_ptr(frame_ptr), m_spheres_ptr(spheres_ptr){}
    void operator()(sycl::item<2> item) const{
        // get our Ids
        const auto x_coord = item.get_id(0);
        const auto y_coord = item.get_id(1);
        // map the 2D indices to a single linear, 1D index
        const auto pixel_index = y_coord * width + x_coord;
        float u = static_cast<float>(x_coord) / static_cast<float>(width-1);
        float v = static_cast<float>(y_coord) / static_cast<float>(height-1);
        camera cam;
        ray r = cam.get_ray(x_coord,y_coord);
        //Vec3 final_color;
        Vec3 final_color = color(r, m_spheres_ptr);

        // write final color to the frame buffer global memory
        m_frame_ptr[pixel_index] = final_color;
        //m_frame_ptr[pixel_index] = Vec3(0.5,0.5,0.5);
    }
    private:
    bool hit_world(ray& r, float min, float max, hit_record& rec, const sycl::accessor<sphere, 1, sycl::access::mode::read, sycl::access::target::device> spheres) const {
        auto temp_rec = hit_record{};
        auto hit_anything = false;
        float closest_so_far = max;
        for (auto i = 0; i < num_spheres; i++) {
            if (spheres[i].hit(r, min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }

    Vec3 color(ray& r, const sycl::accessor<sphere, 1, sycl::access::mode::read, sycl::access::target::device>& spheres) const {
        hit_record rec;
        if (hit_world(r, 0.001f, infinity, rec, spheres)) {
            //return Vec3(1.0f,0.0f,0.0f);
            return Vec3(rec.normal[0] + 1.0f, rec.normal[1] + 1.0f, rec.normal[2] + 1.0f)*0.5f;
        }
        Vec3 unit_direction = (r.direction()).unit();
        float hit_pt = 0.5f*(unit_direction[1] + 1.0f);
        return Vec3(1.0f, 1.0f, 1.0f)*(1.0f - hit_pt) + Vec3(0.5f, 0.7f, 1.0f)*hit_pt;
    }

    /* accessor objects */
    sycl::accessor<Vec3, 1, sycl::access::mode::write, sycl::access::target::device> m_frame_ptr;
    sycl::accessor<sphere, 1, sycl::access::mode::read, sycl::access::target::device> m_spheres_ptr;
};

template <int width, int height, int num_spheres>
void render(sycl::queue& queue, Vec3* fb_data, const sphere *spheres) {
  constexpr auto num_pixels = width * height;
  std::clog<<"am in render function now";
  {
  auto frame_buf = sycl::buffer<Vec3, 1>(fb_data, sycl::range<1>(num_pixels));
  auto spheres_buf = sycl::buffer<sphere, 1>(spheres, sycl::range<1>(num_spheres));
  // submit command group on device
  queue.submit([&](sycl::handler& cgh) {
    std::clog<<"can you see me here in queue";
    // get memory access
    auto frame_ptr = frame_buf.get_access<sycl::access::mode::write>(cgh);
    auto spheres_ptr = spheres_buf.get_access<sycl::access::mode::read>(cgh);
    // setup kernel index space
    auto global = sycl::range<2>(width, height);
    //auto local = sycl::range<2>(constants::TileX, constants::TileY);
    //auto index_space = sycl::nd_range<2>(global, local);
    // construct kernel functor
    // execute kernel
    std::clog<<"\nooo only the parallel for is left hmm\n";
    //cgh.parallel_for(index_space, render_k);
    cgh.parallel_for(global, render_kernel<width, height, num_spheres>(frame_ptr, spheres_ptr));
    std::clog<<"\nooo i wonder what happened hmm\n";
  });
 }
  std::clog<<"hhhuh\n";
  queue.wait();
}
#endif
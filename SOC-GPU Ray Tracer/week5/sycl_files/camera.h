#ifndef CAMERA_H
#define CAMERA_H
#include "rtweekend.h"
#include "sphere.h"

static constexpr auto TileX = 8;
static constexpr auto TileY = 8;

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
    ray get_ray(float u, float v) const {
        return ray(center, pixel00_loc + pixel_delta_u*u + pixel_delta_v*v - center);
    }
};

template <int width, int height>
class render_init_kernel {
  template <typename data_t>
  using write_accessor_t = sycl::accessor<data_t, 1, sycl::access::mode::write,
                                          sycl::access::target::global_buffer>;

 public:
  render_init_kernel(write_accessor_t<xorwow_state_t> rand_states_ptr)
      : m_rand_states_ptr(rand_states_ptr) {}

  void operator()(sycl::nd_item<2> item) const {
    const auto x_coord = item.get_global_id(0);
    const auto y_coord = item.get_global_id(1);

    const auto pixel_index = y_coord * width + x_coord;

    const auto state = get_initial_xorwow_state(pixel_index);
    m_rand_states_ptr[pixel_index] = state;
  }

 private:
  write_accessor_t<xorwow_state_t> m_rand_states_ptr;
};

template <int width, int height, int samples, int num_spheres, class hittable> 
class render_kernel {
    public:
    render_kernel(sycl::accessor<Vec3, 1, sycl::access::mode::write, sycl::access::target::device> frame_ptr, sycl::accessor<hittable, 1, sycl::access::mode::read, sycl::access::target::device> hittables_ptr, sycl::accessor<camera, 1, sycl::access::mode::read, sycl::access::target::device> camera_ptr, sycl::accessor<xorwow_state_t, 1, sycl::access::mode::read_write, sycl::access::target::device> rand_state_ptr): m_frame_ptr(frame_ptr), m_hittables_ptr(hittables_ptr), m_camera_ptr(camera_ptr), m_rand_state_ptr(rand_state_ptr){}
    void operator()(sycl::item<2> item) const{
        // get our Ids
        const auto x_coord = item.get_id(0);
        const auto y_coord = item.get_id(1);
        // map the 2D indices to a single linear, 1D index
        const auto pixel_index = y_coord * width + x_coord;
        //float u = static_cast<float>(x_coord) / static_cast<float>(width-1);
        //float v = static_cast<float>(y_coord) / static_cast<float>(height-1);
        camera cam;
        auto local_rand_state = m_rand_state_ptr[pixel_index];

        // create a 'rng' function object using a lambda
        auto rng = [](xorwow_state_t* state) { return xorwow(state); };

        // capture the rand generator state -> return a uniform value
        auto randf = [&local_rand_state, rng]() {  return rand_uniform(rng, &local_rand_state); };
        int max_depth = 50;
        // color sampling
        Vec3 final_color(0.0, 0.0, 0.0);
        for (auto i = 0; i < samples; i++) {
            //const auto u = (x_coord + randf()-0.5f) / static_cast<float>(width);
            //const auto v = (y_coord + randf()-0.5f) / static_cast<float>(height);
            const auto u = (x_coord + randf()-0.5f);
            const auto v = (y_coord + randf()-0.5f);
            ray r = m_camera_ptr.get_pointer()->get_ray(u, v);
            final_color = final_color + color(r, m_hittables_ptr, rng, &local_rand_state, max_depth);
        }
        final_color = final_color/static_cast<float>(samples);

        // apply gamma correction
        final_color[0] = sycl::sqrt(final_color[0]);
        final_color[1] = sycl::sqrt(final_color[1]);
        final_color[2] = sycl::sqrt(final_color[2]);

        // store final color
        m_frame_ptr[pixel_index] = final_color;
    }
    /*    ray r = cam.get_ray(x_coord,y_coord);
        //Vec3 final_color;
        Vec3 final_color = color(r, m_spheres_ptr);

        // write final color to the frame buffer global memory
        m_frame_ptr[pixel_index] = final_color;
        //m_frame_ptr[pixel_index] = Vec3(0.5,0.5,0.5);
    }*/
    private:
    bool hit_world(ray& r, float min, float max, hit_record& rec, const sycl::accessor<hittable, 1, sycl::access::mode::read, sycl::access::target::device> hittables) const {
        auto temp_rec = hit_record{};
        auto hit_anything = false;
        float closest_so_far = max;
        for (auto i = 0; i < num_spheres; i++) {
            if (hittables[i].hit(r, min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }

    /*template<class rng_t>
    Vec3 color(ray& r, const sycl::accessor<sphere, 1, sycl::access::mode::read, sycl::access::target::device>& spheres, rng_t rng, xorwow_state_t* local_rand_state, int depth) const {
        hit_record rec;
        if (hit_world(r, 0.001f, infinity, rec, spheres)) {
            //return Vec3(1.0f,0.0f,0.0f);
            return Vec3(rec.normal[0] + 1.0f, rec.normal[1] + 1.0f, rec.normal[2] + 1.0f)*0.5f;
        }
        Vec3 unit_direction = (r.direction()).unit();
        float hit_pt = 0.5f*(unit_direction[1] + 1.0f);
        return Vec3(1.0f, 1.0f, 1.0f)*(1.0f - hit_pt) + Vec3(0.5f, 0.7f, 1.0f)*hit_pt;
    }*/
    template <class rng_t>
    Vec3 color(ray& r, const sycl::accessor<hittable, 1, sycl::access::mode::read, sycl::access::target::device>& hittables,
            rng_t rng, xorwow_state_t* local_rand_state, int depth) const{
    ray cur_ray = r;
    Vec3 cur_attenuation(1.0f, 1.0f, 1.0f);
    ray scattered;
    for (auto i = 0; i < depth; i++) {
        hit_record rec;
        if (hit_world(cur_ray, 0.001f, infinity, rec, hittables)) {
        /*Vec3 target = rec.p + rec.normal + rec.normal.random_unit_vector(rng, local_rand_state);
        cur_attenuation = cur_attenuation*0.5f;
        cur_ray = ray(rec.p, target-rec.p);*/
        Vec3 temp_attenuation;
        if (rec.scatter_material(cur_ray,rec,temp_attenuation,scattered,rng,local_rand_state)){
            cur_attenuation = Vec3(temp_attenuation[0]*cur_attenuation[0],temp_attenuation[1]*cur_attenuation[1],temp_attenuation[2]*cur_attenuation[2]);
            cur_ray = scattered;
        }
        else{
            return Vec3(0.0f,0.0f,0.0f);
        }
        }
        else {
        Vec3 unit_direction = cur_ray.direction().unit();
        float t = (unit_direction[1]+ 1.0f)*0.5f;
        Vec3 c = Vec3(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vec3(0.5f, 0.7f, 1.0f) * t;
        Vec3 res = Vec3(cur_attenuation[0]*c[0],cur_attenuation[1]*c[1],cur_attenuation[2]*c[2]);
        return res;
        }
    }
    // exceeded recursion ...
    return Vec3(0.0, 0.0, 0.0);
    }

    /* accessor objects */
    sycl::accessor<Vec3, 1, sycl::access::mode::write, sycl::access::target::device> m_frame_ptr;
    sycl::accessor<hittable, 1, sycl::access::mode::read, sycl::access::target::device> m_hittables_ptr;
    sycl::accessor<camera, 1, sycl::access::mode::read, sycl::access::target::device> m_camera_ptr;
    sycl::accessor<xorwow_state_t, 1, sycl::access::mode::read_write, sycl::access::target::device> m_rand_state_ptr;

};

template <int width, int height>
void render_init(sycl::queue& queue, xorwow_state_t* rand_states) {
  constexpr auto num_pixels = width * height;
  // allocate memory on device
  auto rand_states_buf = sycl::buffer<xorwow_state_t, 1>(rand_states, sycl::range<1>(num_pixels));
  // submit command group on device
  queue.submit([&](sycl::handler& cgh) {
    // get memory access
    auto rand_states_ptr = rand_states_buf.get_access<sycl::access::mode::write>(cgh);
    // setup kernel index space
    const auto global = sycl::range<2>(width, height);
    const auto local = sycl::range<2>(TileX, TileY);
    const auto index_space = sycl::nd_range<2>(sycl::range<2>(width, height), sycl::range<2>(TileX, TileY));
    // construct kernel functor
    auto render_init_func = render_init_kernel<width, height>(rand_states_ptr);
    // execute kernel
    cgh.parallel_for(index_space, render_init_func);
  });
}

template <int width, int height, int samples, int num_spheres, class hittable>
void render(sycl::queue& queue, Vec3* fb_data, const hittable *hittables, camera* cam, xorwow_state_t* rand_states) {
  constexpr auto num_pixels = width * height;
  std::clog<<"am in render function now";
  {
  auto frame_buf = sycl::buffer<Vec3, 1>(fb_data, sycl::range<1>(num_pixels));
  auto spheres_buf = sycl::buffer<hittable, 1>(hittables, sycl::range<1>(num_spheres));
  auto camera_buf = sycl::buffer<camera, 1>(cam, sycl::range<1>(1));
  auto rand_states_buf = sycl::buffer<xorwow_state_t, 1>(rand_states, sycl::range<1>(num_pixels));
  // submit command group on device
  queue.submit([&](sycl::handler& cgh) {
    std::clog<<"can you see me here in queue";
    // get memory access
    auto frame_ptr = frame_buf.get_access<sycl::access::mode::write>(cgh);
    auto spheres_ptr = spheres_buf.template get_access<sycl::access::mode::read>(cgh);
    auto camera_ptr = camera_buf.get_access<sycl::access::mode::read>(cgh);
    auto rand_states_ptr = rand_states_buf.get_access<sycl::access::mode::read_write>(cgh);
    // setup kernel index space
    auto global = sycl::range<2>(width, height);
    //auto local = sycl::range<2>(constants::TileX, constants::TileY);
    //auto index_space = sycl::nd_range<2>(global, local);
    // construct kernel functor
    // execute kernel
    std::clog<<"\nooo only the parallel for is left hmm\n";
    //cgh.parallel_for(index_space, render_k);
    cgh.parallel_for(global, render_kernel<width, height, samples, num_spheres, hittable>(frame_ptr, spheres_ptr, camera_ptr, rand_states_ptr));
    std::clog<<"\nooo i wonder what happened hmm\n";
  });
 }
  std::clog<<"hhhuh\n";
  queue.wait();
}
#endif
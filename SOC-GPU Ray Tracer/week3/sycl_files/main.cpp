#include "rtweekend.h"
#include <sycl/sycl.hpp>
#include "camera.h"
#include "sphere.h"

/*template <int width, int height>
void save_image(Vec3* fb_data) {
  std::cout << "P3\n" << width << " " << height << "\n255\n";
  for (int y = height - 1; y >= 0; y--) {
    for (int x = 0; x < width; x++) {
      auto pixel_index = y * width + x;
      int r = static_cast<int>(255.99 * fb_data[pixel_index][0]);
      int g = static_cast<int>(255.99 * fb_data[pixel_index][1]);
      int b = static_cast<int>(255.99 * fb_data[pixel_index][2]);
      std::cout << r << " " << g << " " << b << "\n";
    }
  }
}*/

int main() {
    // frame buffer dimensions
    constexpr auto width = 640;
    constexpr auto height = 480;
    constexpr auto num_pixels = width * height;

    // select the SYCL accelerator (i.e Intel GPU for this machine)
    sycl::queue queue;
    std::clog << "Using device: " << queue.get_device().get_info<sycl::info::device::name>() <<"\n";
    std::clog<<"Hello I am here";
    // allocate the frame buffer on the Host
    std::vector<Vec3> fb(num_pixels); // frame buffer
    constexpr int num_spheres = 2;
    std::vector<sphere> spheres = {sphere(Vec3(0.0f, 0.0f, -1.0f), 0.5f), sphere(Vec3(0.0f, -100.5f, -1.0f), 100.0f)};
    try{
        std::clog<<"pineapple";
        render<width, height, num_spheres>(queue, fb.data(), spheres.data());
        std::clog << "hello?";
    } catch (sycl::exception const& e) {
        std::cerr << "SYCL Exception caught: " << e.what() << std::endl;
        std::cerr << "Device: " << queue.get_device().get_info<sycl::info::device::name>() << std::endl;
        return 1; // Indicate error
    } catch (const std::exception& e) {
        std::cerr << "Standard Exception caught: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught." << std::endl;
        return 1;
    }
    // save the pixel data as an image file
    std::cout << "P3\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
        auto pixel_index = y * width + x;
        int r = static_cast<int>(255.99f * fb.data()[pixel_index][0]);
        int g = static_cast<int>(255.99f * fb.data()[pixel_index][1]);
        int b = static_cast<int>(255.99f * fb.data()[pixel_index][2]);
        std::cout << r << " " << g << " " << b << "\n";
        }
    }

    return 0;
}
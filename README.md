# SOC_2025-GPU-based-Ray-Tracer
Shanmathi Vasudevan 24B1020 Indian Institute of Technology Bombay
Seasons of Code project 2025, offered by Web and Coding Club, IIT Bombay
This project aims to build a ray tracer optimised for GPU acceleration. Ray tracing is used in computer graphics to produce realistic images by tracing the path of light from the camera to the light sources. Due to its computational nature, we should use GPUs in order to render the images quickly.
My main learnings so far have been
- Basics of ray tracing: Understanding the basic maths behind the ray tracing concept; we wish to find the points where a ray intersects an object, by writing their equations in the 3D space. This simulates how the light interacts; if it hits an object, it illuminates it, and if not, it just shows the background. We can create gradient with a simple formula of colour1 x a + colour2 x (1-a), where a ranges from -1 to 1.
- Smart pointers in C++: Normal pointers in C++ have memory related issues which are hard to recover from and severely impact performance; mainly because they don't get deleted on their own and can land up in places we don't expect them to be (and hence forget to delete). Smart pointers are created to overcome this issue. In the ray tracer program (without sycl), we use shared pointers so that many objects can point to the same memory and the memory gets deleted when nothing is pointing to it.
- SYCL: In order to use the GPU power, we learnt the basics of the SYCL language. It works with both Intel and AMD GPUs. It allows for parallel computation which is very important in ray tracing because we have to calculate the light path, intersections and consequent colour for every pixel in the image.
- Anti-aliasing: In order for the image to not look so jagged, we use anti-aliasing, which takes random samples from pixels around the one we need and averages out the colour. It makes the image more smooth, if the randomness is done properly.
- Adding material properties: For diffuse object, we can have a simple diffusion or a Lambertian reflection. For glass, we can use a Schlick approximation to create the SIR effect. We learnt about linear space and gamma space, and how to correct our linear space code to work in gamma space imag viewers. 

So far, the weekly tasks have been:
- Week 1: Understanding the maths behind ray tracing, implementing a Vec3 class, making a Makefile for the project and creating a simple interpolation ppm image using C++.
- Week 2: Understanding smart pointers in C++, installing the Intel OneAPI toolkit.
- Week 3-4: Creating a ray-traced image of a sphere using our Vec3 class, understanding SYCL fundamentals, implementing the sphere renderer in SYCL. Week 3 took 2 weeks of time as it was hard to understand SYCL.
- Week 5-6: Adding material properties and antialiasing to our spheres. Porting the same to SYCL. This task also took 2 weeks due to the complications involved in implementing the same in SYCL. More specifically, the static polymorphism concept used in SYCL in contrast to the dynamic polymorphism in normal C++, made the code for SYCL more tough to implement as each object property had to be hardcoded into afunction instead of having its own class.

The final images in this repository can be seen in week5; one for the cpu implementation and one in the sycl_files. On my system, the CPU image took 78 seconds to render, whereas the GPU(SYCL) image took 13 seconds to render. However, the SYCL implementation had a less robust randomness function compared to the CPU implementation, leading to that image having more noise.

References:
- https://raytracing.github.io/books/RayTracingInOneWeekend.html For the entire CPU implementation. We have done till chapter 11 here.
- https://codeplay.com/portal/blogs/2020/05/19/ray-tracing-in-a-weekend-with-sycl-basic-sphere-tracing For the SYCL implementation in week3.
- https://www.codeplay.com/portal/blogs/2020/06/19/ray-tracing-in-a-weekend-with-sycl-part-2-pixel-sampling-and-material-tracing.html For the SYCL implementation in week5.

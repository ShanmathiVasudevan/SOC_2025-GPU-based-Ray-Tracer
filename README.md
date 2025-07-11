# SOC_2025-GPU-based-Ray-Tracer
This project aims to build a ray tracer optimised for GPU acceleration. Ray tracing is used in computer graphics to produce realistic images by tracing the path of light from the camera to the light sources. Due to its computational nature, we should use GPUs in order to render the images quickly.
My main learnings so far have been
- Basics of ray tracing: Understanding the basic maths behind the ray tracing concept; we wish to find the points where a ray intersects an object, by writing their equations in the 3D space. This simulates how the light interacts; if it hits an object, it illuminates it, and if not, it just shows the background. We can create gradient with a simple formula of colour1*a+colour2*(1-a), where a ranges from -1 to 1.
- Smart pointers in C++: Normal pointers in C++ have memory related issues which are hard to recover from and severely impact performance; mainly because they don't get deleted on their own and can land up in places we don't expect them to be (and hence forget to delete). Smart pointers are created to overcome this issue. In the ray tracer program (without sycl), we use shared pointers so that many objects can point to the same memory and the memory gets deleted when nothing is pointing to it.
- SYCL: In order to use the GPU power, we learnt the basics of the SYCL language. It works with both Intel and AMD GPUs. It allows for parallel computation which is very important in ray tracing because we have to calculate the light path, intersections and consequent colour for every pixel in the image.

So far, the weekly tasks have been:
- Week 1: Understanding the maths behind ray tracing, implementing a Vec3 class, making a Makefile for the project and creating a simple interpolation ppm image using C++.
- Week 2: Understanding smart pointers in C++, installing the Intel OneAPI toolkit.
- Week 3: Creating a ray-traced image of a sphere using our Vec3 class, understanding SYCL fundamentals, implementing the sphere renderer in SYCL. Week 3 took 2 weeks of time as it was hard to understand SYCL.

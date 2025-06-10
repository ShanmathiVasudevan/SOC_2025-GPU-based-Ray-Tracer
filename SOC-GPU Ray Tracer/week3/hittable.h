#ifndef HITTABLE_H
#define HITTABLE_H
#include "rtweekend.h"
#include "ray.h"

class hit_record {
  public:
    point3 p;
    Vec3 normal;
    double t;
    bool front_face;

    void set_face_normal(ray& r,Vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = r.direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(ray& r, interval ray_t, hit_record& rec) = 0;
};

#endif
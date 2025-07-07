#ifndef SPHERE_H
#define SPHERE_H
#include <sycl/sycl.hpp>
#include "vec3.h"
#include "hittable.h"
/*class hit_record {
  public:
    point3 p;
    Vec3 normal;
    float t;
    bool front_face;

    void set_face_normal(ray& r,Vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = r.direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};*/

class sphere : public hittable<sphere>{
  public:
    sphere() = default;
    sphere(point3 center1, float radius1, material_t mat_type, Vec3 colorr) : center(center1), radius(sycl::fmax(0.0f,radius1)), material_type(mat_type), albedo(colorr) {}
    sphere(point3 center1, float radius1, material_t mat_type, Vec3 colorr, float f) : center(center1), radius(sycl::fmax(0.0f,radius1)), material_type(mat_type), albedo(colorr), fuzz(f) {}
    sphere(point3 center1, float radius1, material_t mat_type, float ref_idx) : center(center1), radius(sycl::fmax(0.0f,radius1)), material_type(mat_type), refraction_index(ref_idx) {}


    bool hit(ray r, float ray_tmin, float ray_tmax, hit_record& rec) const{
        Vec3 oc = center - r.origin();
        auto a = r.direction().length()*r.direction().length();
        auto h = r.direction().dot(oc);
        auto c = oc.length()*oc.length() - radius*radius;

        float discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = sycl::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (root <= ray_tmin || ray_tmax<= root) {
            root = (h + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax<= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        rec.material_type = material_type;
        rec.albedo = albedo;
        rec.fuzz = fuzz;
        rec.refraction_index = refraction_index;
        return true;
    }

  private:
    point3 center;
    float radius;
    material_t material_type;
    Vec3 albedo;
    float fuzz;
    float refraction_index;
};

#endif
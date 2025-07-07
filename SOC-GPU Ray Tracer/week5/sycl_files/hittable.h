#ifndef HITTABLE_H
#define HITTABLE_H
#include "rtweekend.h"
#include "ray.h"

enum class material_t { Lambertian, Metal, Dielectric };

class hit_record {
  public:
    point3 p;
    Vec3 normal;
    float t;
    bool front_face;

    static float reflectance(float cosine, float refraction_index) {
        // Use Schlick's approximation for reflectance.
        float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
        r0 = r0*r0;
        return r0 + (1.0f-r0)*(1.0f - cosine)*(1.0f - cosine)*(1.0f - cosine)*(1.0f - cosine)*(1.0f - cosine);
    }

    void set_face_normal(ray& r,Vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = r.direction().dot(outward_normal) < 0.0f;
        normal = front_face ? outward_normal : -outward_normal;
    }
    hit_record() = default;

    template <class rng_t>
    bool scatter_material(ray& r_in, hit_record& rec, Vec3& attenuation, ray& scattered, rng_t rng, xorwow_state_t* local_rand_state){
      switch (material_type) {
        case material_t::Lambertian:
        {
          // evaluate the lambertian material type
          //[...]
          auto scatter_direction = rec.normal + rec.normal.random_unit_vector(rng, local_rand_state);
          // Catch degenerate scatter direction
          if (scatter_direction.near_zero()){
              scatter_direction = rec.normal;
          }
          scattered = ray(rec.p, scatter_direction);
          attenuation = albedo;
          return true;
        }
        case material_t::Metal:
        {
          // evaluate the metal material type
          //[...]
          Vec3 reflected = r_in.direction().reflect(rec.normal);
          reflected = reflected.unit() + (reflected.random_unit_vector(rng,local_rand_state) * fuzz);
          scattered = ray(rec.p, reflected);
          attenuation = albedo;
          return true;
        }
        case material_t::Dielectric:
        {
          // evaluate the dielectric material type
          //[...]
          attenuation = Vec3(1.0f, 1.0f, 1.0f);
          float ri = rec.front_face ? (1.0f/refraction_index) : refraction_index;

          Vec3 unit_direction = r_in.direction().unit();
          float cos_theta = sycl::fmin(-unit_direction.dot(rec.normal), 1.0f);
          float sin_theta = sycl::sqrt(1.0f - cos_theta*cos_theta);

          bool cannot_refract = ri * sin_theta > 1.0f;
          Vec3 direction;

          if (cannot_refract || reflectance(cos_theta, ri) > rand_uniform(rng, local_rand_state))
              direction = unit_direction.reflect(rec.normal);
          else
              direction = unit_direction.refract(rec.normal, ri);

          scattered = ray(rec.p, direction);
          return true;
        }
        default:
          /* cannot reach here since a material must be defined
          * when upon initialization of a geometry (sphere). */
          return false;
      }
    }

    Vec3 center;
    float radius;

    // material properties
    material_t material_type;
    Vec3 albedo;
    float fuzz;
    float refraction_index;
};

/*class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(ray& r, interval ray_t, hit_record& rec) = 0;
};*/
template <class derived>
struct crtp {
  derived& underlying() { return static_cast<derived&>(*this); }
  const derived& underlying() const {
    return static_cast<const derived&>(*this);
  }
};

template <class geometry>
class hittable : crtp<geometry> {
 public:
  bool hit(const ray& r, float min, float max, hit_record& rec) const {
    return this->underlying().hit(r, min, max, rec);
  }
};

#endif
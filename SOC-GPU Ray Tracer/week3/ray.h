#ifndef RAY_H
#define RAY_H
#include "vec3.h"
using point3 = Vec3;
class ray {
  public:
    ray() {}

    ray(const point3& origin, const Vec3& direction) : orig(origin), dir(direction) {}

    point3& origin() { return orig; }
    Vec3& direction() { return dir; }

    point3 at(double t) {
        return orig + dir*t;
    }

  private:
    point3 orig;
    Vec3 dir;
};
#endif
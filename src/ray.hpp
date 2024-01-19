#pragma once

#include "vec3.hpp"

struct Ray {
    Vec3 orig;
    Vec3 dir;

    Ray() { }
    Ray(const Vec3 &origin, const Vec3 &direction) : orig(origin), dir(direction) { }

    inline Vec3 at(double t) const {
        return orig + t * dir;
    }

    inline void operator =(const Ray &a) {
        orig = a.orig;
        dir = a.dir;
    }
};
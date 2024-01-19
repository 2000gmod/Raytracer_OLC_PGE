#pragma once

#include "vec3.hpp"
#include "ray.hpp"

struct Camera {
    Vec3 
        origin,
        lowerLeftCorner,
        horizontal,
        vertical,
        u, v, w,
        vUp;
    double lensRadius;
    double aspect;

    int lensSides = 2;

    Camera() = default;
    Camera(const Vec3 &lookFrom, const Vec3 &lookAt, const Vec3 &vUp, double fov, double aspect, double aperture, double focusDist);
    Ray getRay(double s, double t) const;

    void updateParams(const Vec3 &pos, const Vec3 &lookDir, double fov, double focusDist, double aperture);
};
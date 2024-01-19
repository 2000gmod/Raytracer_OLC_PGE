#pragma once

#include "vec3.hpp"
#include "ray.hpp"
#include <memory>

struct Material;

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    double t;
    bool frontFace;
    std::shared_ptr<Material> mat;

    inline void setFaceNormal(const Ray &ray, const Vec3 &outwardNormal) {
        frontFace = dot(ray.dir, outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};
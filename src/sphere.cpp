#include "hittable.hpp"
#include <cmath>

bool Sphere::hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const {
    //rec = {};

    Vec3 oc = ray.orig - pos;
    auto a = ray.dir.mag2();
    auto bhalf = dot(oc, ray.dir);
    auto c = oc.mag2() - radius * radius;

    auto discr = bhalf * bhalf - a * c;
    if (discr < 0) return false;
    auto sqrtd = std::sqrt(discr);

    auto root = (-bhalf - sqrtd) / a;
    if (root < minT || maxT < root) {
        root = (-bhalf + sqrtd) / a;
        if (root < minT || maxT < root) {
            return false;
        }
    }

    rec.t = root;
    rec.point = ray.at(rec.t);
    Vec3 outNormal = (rec.point - pos) / radius;
    rec.setFaceNormal(ray, outNormal);
    rec.mat = mat;
    return true;
}
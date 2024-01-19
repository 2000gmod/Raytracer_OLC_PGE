#include "hittable.hpp"

#include <limits>

bool Triangle::hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const {
    auto edgeAB = b - a;
    auto edgeAC = c - a;
    auto normal = cross(edgeAB, edgeAC);
    auto ao = ray.orig - a;
    auto dao = cross(ao, ray.dir);

    auto det = -dot(ray.dir, normal);
    auto invDet = 1 / det;

    auto dst = dot(ao, normal) * invDet;
    auto u = dot(edgeAC, dao) * invDet;
    auto v = -dot(edgeAB, dao) * invDet;
    auto w = 1 - u - v;

    if (dst < minT || dst > maxT) return false;

    if (std::fabs(det) >= 0.00001 && dst >= 0 && u >= 0 && v >= 0 && w >= 0) {
        rec = {};
        rec.point = ray.at(dst);
        rec.setFaceNormal(ray, normal.normalized());
        rec.t = dst;
        rec.mat = mat;
        return true; 
    }
    else return false;
}
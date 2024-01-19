#pragma once

#include "hitrecord.hpp"
#include "material.hpp"
#include "aabb.hpp"
#include <memory>
#include <vector>

struct Hittable {
    virtual bool hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const = 0;

    virtual ~Hittable() = default;
    virtual AABB boundingBox() const = 0;
};

typedef std::shared_ptr<Hittable> HittableSP;

struct HittableList : public Hittable {
    std::vector<HittableSP> objects;

    virtual bool hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const;
    void add(HittableSP obj);

    AABB boundingBox() const override {
        return bbox;
    }

    AABB bbox;
};

struct Sphere : public Hittable {
    Vec3 pos;
    double radius;
    MaterialSP mat;
    AABB bbox;

    Sphere(Vec3 center, double r, MaterialSP mat) : pos(center), radius(r), mat(mat) {
        auto rvec = Vec3(radius, radius, radius);
        bbox = AABB(center - rvec, center + rvec);
    }
    virtual bool hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const;

    AABB boundingBox() const override {
        return bbox;
    }
};

struct Triangle : public Hittable {
    Vec3 a, b, c;
    MaterialSP mat;
    AABB bbox;

    Triangle(Vec3 a, Vec3 b, Vec3 c, MaterialSP mat) : a(a), b(b), c(c), mat(mat) {
        recalculateBBox();
    }

    virtual bool hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const;

    AABB boundingBox() const override {
        return bbox;
    }

    void recalculateBBox() {
        auto minX = std::min(a.x, std::min(b.x, c.x));
        auto minY = std::min(a.y, std::min(b.y, c.y));
        auto minZ = std::min(a.z, std::min(b.z, c.z));

        auto maxX = std::max(a.x, std::max(b.x, c.x));
        auto maxY = std::max(a.y, std::max(b.y, c.y));
        auto maxZ = std::max(a.z, std::max(b.z, c.z));

        bbox = AABB({minX, minY, minZ}, {maxX, maxY, maxZ}).pad();
    }
};
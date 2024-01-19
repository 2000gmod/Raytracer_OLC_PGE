#pragma once

#include "hittable.hpp"
#include "util.hpp"

class BVHNode : public Hittable {
    public:
        BVHNode(const HittableList &list) : BVHNode(list.objects, 0, list.objects.size()) { }

        BVHNode(const std::vector<std::shared_ptr<Hittable>> &srcObjects, size_t start, size_t end);

        AABB boundingBox() const override;
        bool hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const override;

    private:
        std::shared_ptr<Hittable> left, right;
        AABB bbox;

        static bool boxCompare(const std::shared_ptr<Hittable> &a, const std::shared_ptr<Hittable> &b, int axisIndex);
};
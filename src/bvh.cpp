#include "bvh.hpp"

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>> &srcObjects, size_t start, size_t end) {
    auto objects = srcObjects;
    int axis = randXor() % 3;

    auto comparator = [axis] (const std::shared_ptr<Hittable> &a, const std::shared_ptr<Hittable> &b) {
        return boxCompare(a, b, axis);
    };

    size_t objectsSpan = end - start;

    if (objectsSpan == 1) {
        left = right = objects[start];
    }
    else if (objectsSpan == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            left = objects[start];
            right = objects[start + 1];
        }
        else {
            left = objects[start + 1];
            right = objects[start];
        }
    }
    else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        auto mid = start + objectsSpan / 2;
        left = makeSP<BVHNode>(objects, start, mid);
        right = makeSP<BVHNode>(objects, mid, end);
    }

    bbox = AABB(left->boundingBox(), right->boundingBox());
}

AABB BVHNode::boundingBox() const {
    return bbox;
}

bool BVHNode::hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const {
    if (!bbox.hit(ray, Interval(minT, maxT))) {
        return false;
    }

    bool hitL = left->hit(ray, minT, maxT, rec);
    bool hitR = right->hit(ray, minT, hitL ? rec.t : maxT, rec);

    return hitL || hitR;
}

bool BVHNode::boxCompare(const std::shared_ptr<Hittable> &a, const std::shared_ptr<Hittable> &b, int axisIndex) {
    return a->boundingBox().axis(axisIndex).min < b->boundingBox().axis(axisIndex).min;
}
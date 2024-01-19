#include "hitrecord.hpp"
#include "hittable.hpp"

#include <iostream>


bool HittableList::hit(const Ray &ray, double minT, double maxT, HitRecord &rec) const {
    HitRecord temp;
    bool hitAnything = false;
    auto closest = maxT;
    for (auto obj : objects) {
        if (obj->hit(ray, minT, closest, temp)) {
            hitAnything = true;
            closest = temp.t;
            rec = temp;
        }
    }
    return hitAnything;
}

void HittableList::add(HittableSP obj) {
    objects.push_back(obj);
    bbox = AABB(bbox, obj->boundingBox());
}
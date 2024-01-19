#include "aabb.hpp"

AABB operator +(const AABB &box, const Vec3 &offset) {
    return AABB(box.x + offset.x, box.y + offset.y, box.z + offset.z);
}

AABB operator +(const Vec3 &offset, const AABB &box) {
    return box + offset;
}
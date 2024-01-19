#pragma once

#include "interval.hpp"
#include "vec3.hpp"
#include "ray.hpp"

class AABB {
    public:
        Interval x, y, z;

        AABB() { }
        AABB(const Interval &ix, const Interval &iy, const Interval &iz) : x(ix), y(iy), z(iz) { }

        AABB(const Vec3 &a, const Vec3 &b) {
            x = Interval(std::fmin(a.x, b.x), std::fmax(a.x, b.x));
            y = Interval(std::fmin(a.y, b.y), std::fmax(a.y, b.y));
            z = Interval(std::fmin(a.z, b.z), std::fmax(a.z, b.z));
        }

        AABB(const AABB &b0, const AABB &b1) {
            x = Interval(b0.x, b1.x);
            y = Interval(b0.y, b1.y);
            z = Interval(b0.z, b1.z);
        }

        AABB pad() const {
            double delta = 0.0001;
            Interval nX = (x.size() >= delta) ? x : x.expand(delta);
            Interval nY = (y.size() >= delta) ? y : y.expand(delta);
            Interval nZ = (z.size() >= delta) ? z : z.expand(delta);

            return AABB(nX, nY, nZ);
        }

        const Interval &axis(int n) const {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }

        bool hit(const Ray &r, Interval rayT) const {
            for (int a = 0; a < 3; a++) {
                auto invD = 1 / r.dir[a];
                auto orig = r.orig[a];

                auto t0 = (axis(a).min - orig) * invD;
                auto t1 = (axis(a).max - orig) * invD;

                if (invD < 0) {
                    std::swap(t0, t1);
                }

                if (t0 > rayT.min) rayT.min = t0;
                if (t1 < rayT.max) rayT.max = t1;

                if (rayT.max <= rayT.min) return false;
            }
            return true;
        }
};

AABB operator +(const AABB &box, const Vec3 &offset);
AABB operator +(const Vec3 &offset, const AABB &box);
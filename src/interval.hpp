#pragma once

#include <limits>
#include <cmath>

class Interval {
    public:
        double min, max;
        static const Interval empty, universe;

        Interval() : min(std::numeric_limits<double>::infinity()), max(-std::numeric_limits<double>::infinity()) { }
        Interval(double min, double max) : min(min), max(max) { }
        Interval(const Interval &a, const Interval &b) {
            min = std::fmin(a.min, b.min);
            max = std::fmax(a.max, b.max);
        }

        double size() const {
            return max - min;
        }

        Interval expand(double delta) const {
            auto padding = delta / 2;
            return Interval(min - padding, max + padding);
        }

        bool contains(double x) const {
            return min <= x && x <= max;
        }

        bool surrounds(double x) const {
            return min < x && x < max;
        }

        double clamp(double x) {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }
};

Interval operator +(const Interval &ival, double disp);
Interval operator +(double disp, const Interval &ival);
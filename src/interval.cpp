#include "interval.hpp"

const Interval Interval::empty = Interval(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
const Interval Interval::universe = Interval(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

Interval operator +(const Interval &ival, double disp) {
    return Interval(ival.min + disp, ival.max + disp);
}

Interval operator +(double disp, const Interval &ival) {
    return ival + disp;
}
#include "vec3.hpp"
#include <cmath>

double Vec3::mag2() const {
    return x * x + y * y + z * z;
}

double Vec3::mag() const {
    return std::sqrt(mag2());
}

Vec3 Vec3::normalized() const {
    return *this / mag();
}

bool Vec3::nearZero() const {
    auto s = 1e-8;
    return (std::abs(x) < s && std::abs(y) < s && std::abs(z) < s);
}

void Vec3::operator =(const Vec3 &a) {
    x = a.x;
    y = a.y;
    z = a.z;
}

void Vec3::operator +=(const Vec3 &a) {
    *this = *this + a;
}

double dot(const Vec3 &a, const Vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(const Vec3 &u, const Vec3 &v) {
    return {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
}

Vec3 operator -(const Vec3 &a) {
    return {-a.x, -a.y, -a.z};
}

Vec3 operator +(const Vec3 &a, const Vec3 &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec3 operator -(const Vec3 &a, const Vec3 &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 operator *(const Vec3 &a, double b) {
    return {b * a.x, b * a.y, b * a.z};
}

Vec3 operator *(double a, const Vec3 &b) {
    return b * a;
}

Vec3 operator /(const Vec3 &a, double b) {
    return {a.x / b, a.y / b, a.z / b};
}

Vec3 operator *(const Vec3 &a, const Vec3 &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

Vec3 reflect(const Vec3 &v, const Vec3 &normal) {
    return v - 2 * dot(v, normal) * normal;
}

Vec3 refract(const Vec3 &uv, const Vec3 &normal, double etaiOverEtat) {
    auto cosTheta = std::fmin(dot(-uv, normal), 1.0);
    Vec3 rOutPerp = etaiOverEtat * (uv + cosTheta * normal);
    Vec3 rOutParallel = -std::sqrt(std::fabs(1.0 - rOutPerp.mag2())) * normal;
    return rOutPerp + rOutParallel;
}
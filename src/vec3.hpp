#pragma once

struct Vec3 {
    double x, y, z;

    Vec3() : x(0), y(0), z(0) { }
    Vec3(double x, double y, double z) : x(x), y(y), z(z) { }
    Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z) { }

    double mag2() const;
    double mag() const;
    Vec3 normalized() const;

    bool nearZero() const;

    void operator =(const Vec3 &a);
    void operator +=(const Vec3 &a);

    double operator[] (int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }
};

double dot(const Vec3 &a, const Vec3 &b);
Vec3 cross(const Vec3 &a, const Vec3 &b);

Vec3 operator -(const Vec3 &a);

Vec3 operator +(const Vec3 &a, const Vec3 &b);
Vec3 operator -(const Vec3 &a, const Vec3 &b);

Vec3 operator *(const Vec3 &a, double b);
Vec3 operator *(double a, const Vec3 &b);

Vec3 operator /(const Vec3 &a, double b);

Vec3 operator *(const Vec3 &a, const Vec3 &b);

Vec3 reflect(const Vec3 &v, const Vec3 &normal);
Vec3 refract(const Vec3 &uv, const Vec3 &normal, double etaiOverEtat);
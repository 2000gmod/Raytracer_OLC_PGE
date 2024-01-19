#include "mat3.hpp"
#include <cmath>

double Mat3::operator [](int i) const {
    return a[i];
}

double &Mat3::at(int i) {
    return a[i];
}

Mat3 operator *(const Mat3 &a, const Mat3 &b) {
    Mat3 out;

    out.at(0) = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
    out.at(1) = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
    out.at(2) = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];

    out.at(3) = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
    out.at(4) = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
    out.at(5) = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];

    out.at(6) = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
    out.at(7) = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
    out.at(8) = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];

    return out;
}

Vec3 operator *(const Mat3 &a, const Vec3 &b) {
    Vec3 out;

    out.x = dot({a[0], a[1], a[2]}, b);
    out.y = dot({a[3], a[4], a[5]}, b);
    out.z = dot({a[6], a[7], a[8]}, b);
    return out;
}


Mat3 getRotationX(double theta) {
    Mat3 out;
    auto cg = std::cos(theta);
    auto sg = std::sin(theta);

    out.at(0) = cg;
    out.at(1) = -sg;
    out.at(3) = sg;
    out.at(4) = cg;
    out.at(8) = 1.0;

    return out;
}

Mat3 getRotationY(double theta) {
    Mat3 out;
    auto cg = std::cos(theta);
    auto sg = std::sin(theta);

    out.at(0) = cg;
    out.at(2) = sg;
    out.at(4) = 1.0;
    out.at(6) = -sg;
    out.at(8) = cg;

    return out;
}

Mat3 getRotationZ(double theta) {
    Mat3 out;
    auto cg = std::cos(theta);
    auto sg = std::sin(theta);

    out.at(0) = 1.0;
    out.at(4) = cg;
    out.at(5) = -sg;
    out.at(7) = sg;
    out.at(8) = cg;

    return out;
}

Mat3 getRotationMatrix(const Vec3 &rotVec) {
    return getRotationX(rotVec.x) * getRotationY(rotVec.y) * getRotationZ(rotVec.z);
}

Mat3 getScaleMatrix(const Vec3 &scale) {
    Mat3 out;
    out.at(0) = scale.x;
    out.at(4) = scale.y;
    out.at(8) = scale.z;
    return out;
}
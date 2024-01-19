#pragma once

#include "vec3.hpp"

struct Mat3 {
    private:
        double a[9];
    public:
        Mat3() {
            for (int i = 0; i < 9; i++) a[i] = 0;
        }
        
        Mat3(const Mat3 &b) {
            for (int i = 0; i < 9; i++) {
                a[i] = b.a[i];
            }
        }

        double operator [](int i) const;
        double &at(int i);
};

Mat3 operator *(const Mat3 &a, const Mat3 &b);
Vec3 operator *(const Mat3 &a, const Vec3 &b);

Mat3 getRotationX(double theta);
Mat3 getRotationY(double theta);
Mat3 getRotationZ(double theta);

Mat3 getRotationMatrix(const Vec3 &rotVec);

Mat3 getScaleMatrix(const Vec3 &scale);
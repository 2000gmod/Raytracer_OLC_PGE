#include "camera.hpp"
#include "util.hpp"

#include <cmath>

Camera::Camera(const Vec3 &lookFrom, const Vec3 &lookAt, const Vec3 &vUp, double fov, double aspect, double aperture, double focusDist) {
    auto theta = (mathPi / 180.f) * fov;
    auto h = std::tan(theta / 2.f);
    auto viewportHeight = 2 * h;
    auto viewportWidth = aspect * viewportHeight;
    this->vUp = vUp;

    w = (lookFrom - lookAt).normalized();
    u = cross(vUp, w).normalized();
    v = cross(w, u);

    origin = lookFrom;
    horizontal = focusDist * viewportWidth * u;
    vertical = focusDist * viewportHeight * v;
    lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - w * focusDist;

    lensRadius = aperture / 2;
    this->aspect = aspect;
}

Ray Camera::getRay(double s, double t) const {
    auto rd = lensRadius * randomInUnitRegularPoly(lensSides);
    auto offset = u * rd.x + v * rd.y;
    return Ray {origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset};
}

void Camera::updateParams(const Vec3 &pos, const Vec3 &lookDir, double fov, double focusDist, double aperture) {
    auto theta = (mathPi / 180.f) * fov;
    auto h = std::tan(theta / 2.f);
    auto viewportHeight = 2 * h;
    auto viewportWidth = aspect * viewportHeight;
    origin = pos;

    auto lookAt = origin + lookDir;
    w = (pos - lookAt).normalized();
    u = cross(vUp, w).normalized();
    v = cross(w, u);

    
    horizontal = focusDist * viewportWidth * u;
    vertical = focusDist * viewportHeight * v;
    lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - w * focusDist;

    lensRadius = aperture / 2;
}
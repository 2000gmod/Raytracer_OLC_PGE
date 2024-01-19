#include "material.hpp"
#include "util.hpp"
#include "vec3.hpp"

bool Diffuse::scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const {
    auto scatterDir = record.normal + randomUnitVector();

    if (scatterDir.nearZero()) {
        scatterDir = record.normal;
    }

    scattered = {record.point, scatterDir};
    attenuation = albedo;
    return true;
}

bool Diffuse::emit(Vec3 &color) const {
    color = albedo;
    return false;
}

bool Metal::scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const {
    auto reflected = reflect(in.dir.normalized(), record.normal);
    scattered = {record.point, reflected + fuzz * randomInUnitSphere()};
    attenuation = albedo;
    return dot(scattered.dir, record.normal) > 0;
}

bool Metal::emit(Vec3 &color) const {
    color = albedo;
    return false;
}

bool Emmisive::scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const {
    return false;
}

bool Emmisive::emit(Vec3 &color) const {
    color = this->color * str;
    return true;
}

bool Glass::scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const {
    attenuation = albedo;
    double refractionRatio = record.frontFace ? (1.0 / IR) : IR;

    Vec3 unitDir = in.dir.normalized();
    double cosTheta = std::fmin(dot(-unitDir, record.normal), 1.0);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = refractionRatio * sinTheta > 1.0;
    Vec3 dir;

    if (cannotRefract || reflectance(cosTheta, refractionRatio) > randomDouble()) {
        dir = reflect(unitDir, record.normal);
    }
    else {
        dir = refract(unitDir, record.normal, refractionRatio);
    }

    scattered = Ray(record.point, dir);
    return true;
}

bool Glass::emit(Vec3 &color) const {
    return false;
}

double Glass::reflectance(double cosine, double refIdx) {
    auto r0 = (1 - refIdx) / (1 + refIdx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}
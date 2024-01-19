#pragma once

#include "hitrecord.hpp"
#include <memory>

using std::shared_ptr;

struct Material {
    virtual bool scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const = 0;
    virtual bool emit(Vec3 &color) const = 0;

    virtual ~Material() = default;
};

typedef shared_ptr<Material> MaterialSP;

struct Diffuse : public Material {
    Vec3 albedo;
    Diffuse(Vec3 color) : albedo(color) { }
    
    virtual bool scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const override;
    virtual bool emit(Vec3 &color) const override;
};


struct Metal : public Material {
    Vec3 albedo;
    double fuzz;
    Metal(Vec3 color, double fuzz) : albedo(color), fuzz(fuzz) { }
    
    virtual bool scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const override;
    virtual bool emit(Vec3 &color) const override;
};

struct Glass : public Material {
    Vec3 albedo;
    double IR;

    Glass(const Vec3 &color, double IR) : IR(IR), albedo(color) { }
    
    virtual bool scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const override;
    virtual bool emit(Vec3 &color) const override;
    static double reflectance(double cosine, double refIdx);
};

struct Emmisive : public Material {
    Vec3 color;
    double str;
    Emmisive(Vec3 color, double str) : color(color), str(str) { }

    virtual bool scatter(const Ray &in, const HitRecord &record, Vec3 &attenuation, Ray &scattered) const override;
    virtual bool emit(Vec3 &color) const override;
};


#include "util.hpp"
#include "hitrecord.hpp"
#include "bvh.hpp"
#include <sstream>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <random>
#include <thread>

#include <fstream>

static thread_local uint32_t randState = std::hash<std::thread::id>{}(std::this_thread::get_id());

uint32_t randLcg() {
    randState = 1664525 * randState + 1013904223;
    return randState;
}

uint32_t randXor() {
    randState ^= (randState << 13);
    randState ^= (randState >> 17);
    randState ^= (randState << 5);
    return randState;
}

double randomDouble(double a, double b) {
    //thread_local std::default_random_engine rd(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    //static thread_local std::mt19937 rd(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    //std::uniform_real_distribution<> dist(a, b);
    //return dist(rd);
    return a + randomDouble() * (b - a);
}

double randomDouble() {
    //return randomDouble(0, 1);
    //srand(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    return (double) randXor() / UINT32_MAX;
}

Vec3 randomVec(double a, double b) {
    return {randomDouble(a, b), randomDouble(a, b), randomDouble(a, b)};
}

Vec3 randomInUnitSphere() {
    while (1) {
        auto p = randomVec(-1, 1);
        if (p.mag2() >= 1) continue;
        return p;
    }
}

Vec3 randomInUnitDisk() {
    while (1) {
        Vec3 p = {randomDouble(-1, 1), randomDouble(-1, 1), 0};
        if (p.mag2() >= 1) continue;
        return p;
    }
}

Vec3 randomUnitVector() {
    return randomInUnitSphere().normalized();
}

Vec3 pointInSphere(double theta, double phi) {
    return {std::cos(theta), std::sin(phi), std::sin(theta)};
}

uint8_t colorToByte(double col) {
    col *= 255.999;

    col = clamp(col, 0, 255);

    return (uint8_t) std::round(col);
}

olc::Pixel vec3ToPix(const Vec3 &color) {
    return {colorToByte(color.x), colorToByte(color.y), colorToByte(color.z)};
}

Vec3 skyColorFunction(const Vec3 &dir) {
    //return dir.normalized();
    //return Vec3{0.01, 0.01, 0.01};

    const Vec3 sunDir = Vec3{1, 1, -1}.normalized();
    const double solarRadius = 0.25;
    const Vec3 solarColor = Vec3{1, 1, 1} * 5;

    
    auto cosrad = std::cos(solarRadius);

    if (dot(sunDir, dir.normalized()) > cosrad) return solarColor;
    return Vec3{0.2, 0.2, dir.y + 0.4};
    //return {};
}

Vec3 rayColor(const Ray &ray, const std::shared_ptr<Hittable> &world, int depth) {
    if (depth <= 0) {
        return {};
    }

    HitRecord rec;

    if (world->hit(ray, 0.0001, std::numeric_limits<double>::infinity(), rec)) {
        Ray scattered;
        Vec3 attenuation;

        if(rec.mat->scatter(ray, rec, attenuation, scattered)) {
            return attenuation * rayColor(scattered, world, depth - 1);
        }

        if (rec.mat->emit(attenuation)) {
            return attenuation;
        }
    }

    return skyColorFunction(ray.dir);
}

double clamp(double x, double a, double b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

MemoryManager *MemoryManager::manager = nullptr;
//MemoryManager MemoryManager::manager = MemoryManager();

double sign(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pointInTriangle(const Vec3 &p, const Vec3 &v1, const Vec3 &v2, const Vec3 &v3) {
    auto d1 = sign(p, v1, v2);
    auto d2 = sign(p, v2, v3);
    auto d3 = sign(p, v3, v1);

    auto hasNeg = (d1 < 0.0) || (d2 < 0.0) || (d3 < 0.0);
    auto hasPos = (d1 > 0.0) || (d2 > 0.0) || (d3 > 0.0);

    return !(hasNeg && hasPos);
}

Vec3 randomInUnitRegularPoly(int n) {
    if (n <= 2) {
        return randomInUnitDisk();
    }
    const auto pi = mathPi;
    const Vec3 center(0, 0, 0);
    const auto angleOffset = pi / 2;

    Vec3 point;

    do {
        point = randomInUnitDisk();
        for (int i = 0; i < n; i++) {
            auto idb = double(i);
            Vec3 p1 = {std::cos(2 * pi * idb / n + angleOffset), std::sin(2 * pi * idb / n + angleOffset), 0};
            Vec3 p2 = {std::cos(2 * pi * (idb + 1) / n + angleOffset), std::sin(2 * pi * (idb + 1) / n + angleOffset), 0};
            if (pointInTriangle(point, center, p1, p2)) {
                return point;
            }
        }
    } while(true);
}

std::shared_ptr<Hittable> loadFromObj(const char *filepath, const std::shared_ptr<Material> &mat) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filepath << "\n";
        std::exit(1);
    }

    std::vector<Vec3> points;
    std::vector<Triangle> tris;
    std::shared_ptr<HittableList> wlist = makeSP<HittableList>();

    for (std::string line; getline(file, line);) {
        std::stringstream sst;
        sst << line;
        char junk;

        if (line[0] == 'v' && line[1] == ' ') {
            Vec3 p;
            sst >> junk >> p.x >> p.y >> p.z;

            points.push_back(p);
        }
        if (line[0] == 'f') {
            int f[3];
            sst >> junk >> f[0] >> f[1] >> f[2];
            tris.emplace_back(points[f[0] - 1], points[f[1] - 1], points[f[2] - 1], mat);
        }
    }
    for (auto &t : tris) {
        wlist->add(makeSP<Triangle>(t));
    }
    return wlist;
}

std::shared_ptr<Hittable> buildBVHTree(const std::shared_ptr<Hittable> &world) {
    std::cout << "Building scene BVH tree...\n";
    if (dynamic_cast<HittableList*>(world.get()) == nullptr) {
        std::cerr << "Needs to be a flat list\n";
        std::exit(1);
    }
    auto wlist = flattenList(std::dynamic_pointer_cast<HittableList>(world));
    auto out = makeSP<BVHNode>(*wlist);
    std::cout << "BVH tree built, final object count: " << wlist->objects.size() << ".\n";
    return out;
}

std::shared_ptr<HittableList> flattenList(const std::shared_ptr<HittableList> &wlist) {
    auto out = makeSP<HittableList>();

    for (auto &i : wlist->objects) {
        if (dynamic_cast<HittableList*>(i.get()) != nullptr) {
            auto sublist = flattenList(std::dynamic_pointer_cast<HittableList>(i))->objects;
            for (auto &j : sublist) out->add(j);
        }
        else out->add(i);
    }
    return out;
}
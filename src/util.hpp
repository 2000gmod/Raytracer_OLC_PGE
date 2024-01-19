#pragma once

#include <random>
#include <memory>
#include "vec3.hpp"
#include "hittable.hpp"
#include "olc/olcPGE.hpp"

uint32_t randXor();

double randomDouble(double a, double b);
double randomDouble();

Vec3 randomVec(double a, double b);
Vec3 randomInUnitSphere();
Vec3 randomInUnitDisk();
Vec3 randomUnitVector();
Vec3 pointInSphere(double theta, double phi);

olc::Pixel vec3ToPix(const Vec3 &color);

Vec3 rayColor(const Ray &ray, const std::shared_ptr<Hittable> &world, int depth);

const double mathPi = 3.141592653589793;

double clamp(double x, double a, double b);

struct MemoryManager {
    static const size_t maxMem = 5000000;
    uint8_t *mem;
    size_t nextAlloc = 0;

    MemoryManager() {
        mem = new uint8_t[maxMem];
    }

    ~MemoryManager() {
        delete[] mem;
    }

    uint8_t *alloc(size_t size) {
        //std::cout << "MM[" << nextAlloc << ", " << size << "]\n";
        if (nextAlloc + size > maxMem) {
            std::cerr << "Stack memory allocator filled\n";
            std::exit(1);
        }
        auto out = &mem[nextAlloc];
        nextAlloc += size;
        return out;
    }

    static MemoryManager *manager;
};

enum class MemoryStrategy {
    HEAP, ARENA
};

extern MemoryStrategy memStrategy;

template <class T, class... Args>
inline std::shared_ptr<T> makeSP(Args... args) {
    if (memStrategy == MemoryStrategy::HEAP) return std::make_shared<T>(args...);

    static auto df = [] (T *t) {
        t->~T();
    };
    auto *alloc = MemoryManager::manager->alloc(sizeof(T));
    auto ptr = std::shared_ptr<T>(new (alloc) T(args...), df);
    return ptr;
    //return std::make_shared<T>(args...);
}

Vec3 randomInUnitRegularPoly(int n);

std::shared_ptr<Hittable> loadFromObj(const char *filepath, const std::shared_ptr<Material> &mat);
std::shared_ptr<Hittable> buildBVHTree(const std::shared_ptr<Hittable> &world);
std::shared_ptr<HittableList> flattenList(const std::shared_ptr<HittableList> &wlist);

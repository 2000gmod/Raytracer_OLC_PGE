#include "mainscreen.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include "bvh.hpp"
#include "sceneloading.hpp"

#include <cmath>
#include <exception>
#include <string>

using namespace olc;

MainScreen::MainScreen(uint32_t tileSize, int numThreads, const std::string &sceneName) : pool(numThreads), sceneName(sceneName) {
    sAppName = "Raytracer";
    processedSamples = 0;
    this->tileSize = tileSize;
    MemoryManager::manager = &manager;
}

MainScreen::~MainScreen() {
    delete[] colorArray;
}

bool MainScreen::OnUserDestroy() {
    std::cout << std::endl;
    return true;
}
 
bool MainScreen::OnUserCreate() {
    camPos = Vec3{-7, 1, 4} / 1;
    auto lookDir = Vec3{-1, 1, -1};
    Vec3 vUp = {0, 1, 0};

    auto size = GetScreenSize();

    cam = {camPos, lookDir, vUp, 60, (double) size.x / (double) size.y, aperture, focusDist};
    colorArray = new Vec3[size.x * size.y];

    maxDepth = 6;

    reloadScene();

    createRenderRegions();
    running = true;
    
    return true;
}

bool MainScreen::OnUserUpdate(float dTime) {
    processKeys(dTime);
    if (!running) return false;
    cam.updateParams(camPos, pointInSphere(theta, phi), fov, focusDist, aperture);
    
    DrawScene();

    drawColorArray();

    if (drawInfo) {
        DrawString({1, 1}, "S: " + std::to_string((int) processedSamples) + ", L:" + std::to_string(cam.lensSides), olc::RED);
        
        auto center = GetScreenSize() / 2;
        auto color = olc::RED;
        DrawLine(center + olc::vi2d{0, 5}, center + olc::vi2d{0, -5}, color);
        DrawLine(center + olc::vi2d{5, 0}, center + olc::vi2d{-5, 0}, color);
    }
    //sAppName = std::to_string(processedSamples);
    return true;
}

void MainScreen::drawToColorArray(int x, int y, const Vec3 &col) {
    auto size = GetScreenSize();
    if (x < 0 || x >= size.x || y < 0 || y >= size.y) {
        std::cout << "Drawing out of bounds: " << x << ", " << y << "\n";
        std::cout << "Max size is: {" << size.x << ", " << size.y << "}\n";
        std::exit(1);
    }
    colorArray[y * size.x + x] = col;
}

Vec3& MainScreen::getColorArray(int x, int y) const {
    auto size = GetScreenSize();
    if (x < 0 || x >= size.x || y < 0 || y >= size.y) {
        std::cout << "Get color out of bounds: " << x << ", " << y << "\n";
        std::cout << "Max size is: {" << size.x << ", " << size.y << "}\n";
        std::exit(1);
    }
    return colorArray[y * size.x + x];
}

void MainScreen::clearColorArray() {
    auto size = GetScreenSize();

    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            drawToColorArray(x, y, {0, 0, 0});
        }
    }
    processedSamples = 0.0;
}


void MainScreen::drawColorArray() {
    auto size = GetScreenSize();
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            Vec3 col;
            
            col = getColorArray(x, y);
            col.x = std::sqrt(col.x);
            col.y = std::sqrt(col.y);
            col.z = std::sqrt(col.z);
            
            Draw({x, size.y - y}, vec3ToPix(col));
        }
    }
    
}

void MainScreen::processKeys(float dTime) {
    if (GetKey(olc::W).bHeld) {
        camPos += pointInSphere(theta, phi) * dTime * 5;
        clearColorArray();
    }
    if (GetKey(olc::S).bHeld) {
        camPos += -pointInSphere(theta, phi) * dTime * 5;
        clearColorArray();
    }

    if (GetKey(olc::A).bHeld) {
        camPos += -cross(pointInSphere(theta, phi), {0, 1, 0}).normalized() * dTime * 5;
        clearColorArray();
    }
    if (GetKey(olc::D).bHeld) {
        camPos += cross(pointInSphere(theta, phi), {0, 1, 0}).normalized() * dTime * 5;
        clearColorArray();
    }

    if (GetKey(olc::SHIFT).bHeld) {
        camPos += Vec3{0, 1, 0} * dTime * 5;
        clearColorArray();
    }
    if (GetKey(olc::CTRL).bHeld) {
        camPos += Vec3{0, -1, 0} * dTime * 5;
        clearColorArray();
    }

    if (GetKey(olc::LEFT).bHeld) {
        theta -= dTime;
        clearColorArray();
    }

    if (GetKey(olc::RIGHT).bHeld) {
        theta += dTime;
        clearColorArray();
    }

    if (GetKey(olc::UP).bHeld) {
        phi += dTime;
        clearColorArray();
    }

    if (GetKey(olc::DOWN).bHeld) {
        phi -= dTime;
        clearColorArray();
    }

    if (GetKey(olc::Z).bHeld) {
        focusDist -= dTime / 2;
        focusDist = focusDist <= 0.0 ? 0.0 : focusDist;
        clearColorArray();
    }

    if (GetKey(olc::X).bHeld) {
        focusDist += dTime / 2;
        clearColorArray();
    }

    if (GetKey(olc::N).bHeld) {
        aperture -= dTime / 4;
        aperture = aperture <= 0.0 ? 0.0 : aperture;
        clearColorArray();
    }

    if (GetKey(olc::M).bHeld) {
        aperture += dTime / 4;
        clearColorArray();
    }

    if (GetKey(olc::ESCAPE).bPressed) {
        running = false;
    }

    if (GetKey(olc::T).bHeld) {
        visualizeRenderRegions = true;
        clearColorArray();
    }
    else if (GetKey(olc::T).bReleased) {
        visualizeRenderRegions = false;
        clearColorArray();
    }

    if (GetKey(olc::Key::L).bPressed) {
        cam.lensSides++;
        clearColorArray();
    }

    if (GetKey(olc::Key::K).bPressed) {
        cam.lensSides = cam.lensSides <= 2 ? 2 : cam.lensSides - 1;
        clearColorArray();
    }

    if (GetKey(olc::Key::F).bPressed) {
        HitRecord rec {};
        auto ray = cam.getRay(0.5, 0.5);
        world->hit(ray, 0.0001, std::numeric_limits<double>::infinity(), rec);
        focusDist = (rec.point - cam.origin).mag();
        std::cout << "\t Refocus distance: " << focusDist << "\n";
        clearColorArray();
    }

    if (GetKey(olc::Key::F1).bPressed) {
        drawInfo = !drawInfo;
    }

    phi = clamp(phi, -mathPi / 2, mathPi / 2);
}


void MainScreen::DrawScene() {
    auto size = GetScreenSize();

    static auto worker = [this, size] (const RenderRegion &reg) {
        Vec3 pixColor;
        if (visualizeRenderRegions) pixColor = Vec3{randomDouble(), randomDouble(), randomDouble()};
        for (int y = reg.y0; y < reg.y1; y++) {
            for (int x = reg.x0; x < reg.x1; x++) {
                
                auto u = (double(x) + randomDouble()) / double(size.x - 1);
                auto v = (double(y) + randomDouble()) / double(size.y - 1);

                Ray r = cam.getRay(u, v);
                if (!visualizeRenderRegions) {
                    pixColor = (getColorArray(x, y) * processedSamples + rayColor(r, world, maxDepth)) / (processedSamples + 1);
                }

                drawToColorArray(x, y, pixColor);
            }
        }
    };

    for (const auto &i : rRegions) {
        pool.Submit(worker, i);
    }
    pool.WaitUntilEmpty();

    processedSamples += 1;
    drawColorArray();
}


void MainScreen::randomScene(int limit) {
    auto wlist = std::dynamic_pointer_cast<HittableList>(world);
    auto groundMat = makeSP<Diffuse>(Vec3{0.5, 0.5, 0.5});
    //wlist->add(makeSP<Sphere>(Vec3{0, -600000, 0}, 600000, groundMat));

    for (int a = -limit; a < limit; a++) for (int b = -limit; b < limit; b++) {
        auto chooseMat = randomDouble();
        Vec3 center {a + 0.9 * randomDouble(), 0.2 - 1, b + 0.9 * randomDouble()};

        if ((center - Vec3 {4, 0.2, 0}).mag() > 0.9) {
            if (chooseMat < 0.7) {
                auto albedo = randomVec(0.01, 0.9);
                auto sphereMat = makeSP<Diffuse>(albedo);
                wlist->add(makeSP<Sphere>(center, 0.2, sphereMat));
            }
            else if (chooseMat < 0.8) {
                auto albedo = randomVec(0.7, 1);
                auto fuzz = randomDouble(0, 0.2);
                auto sphereMat = makeSP<Metal>(albedo, fuzz);
                wlist->add(makeSP<Sphere>(center, 0.2, sphereMat));
            }
            else if (chooseMat < 0.95) {
                auto rindex = randomDouble(1.2, 2);
                auto albedo = randomVec(0.7, 1);
                auto sphereMat = makeSP<Glass>(albedo, rindex);
                wlist->add(makeSP<Sphere>(center, 0.2, sphereMat));
            }
            else {
                auto color = randomVec(0.3, 1).normalized();
                auto str = randomDouble(5, 20);
                auto sphereMat = makeSP<Emmisive>(color, str);
                wlist->add(makeSP<Sphere>(center, 0.2, sphereMat));
            }
        }
    }
}

void MainScreen::createRenderRegions() {
        int currX = 0, currY = 0;
    auto size = GetScreenSize();

    while (currY < size.y) {
        currX = 0;
        while (currX < size.x) {
            RenderRegion region;

            region.x0 = currX;
            if (size.x - currX < tileSize) {
                region.x1 = size.x;
            }
            else {
                region.x1 = currX + tileSize;
            }

            region.y0 = currY;
            if (size.y - currY < tileSize) {
                region.y1 = size.y;
            }
            else {
                region.y1 = currY + tileSize;
            }

            rRegions.push_back(region);
            currX += tileSize;
        }
        currY += tileSize;
    }

    std::cout << "Created " << rRegions.size() << " render regions.\n";
}

void MainScreen::reloadScene() {
    auto wlist = makeSP<HittableList>();
    wlist->add(loadScene(sceneName.c_str()));
    world = buildBVHTree(wlist);
}
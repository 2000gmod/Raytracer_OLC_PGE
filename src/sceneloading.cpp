#include "sceneloading.hpp"
#include "util.hpp"
#include "mat3.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <unordered_map>

using json = nlohmann::json;

static std::unordered_map<std::string, MaterialSP> loadMats(const json &data) {
    std::unordered_map<std::string, MaterialSP> out;

    for (auto &i : data["materials"]) {
        auto type = i["type"];
        if (type == "diffuse") {
            auto albedo = i["albedo"];
            out.insert_or_assign(i["name"], makeSP<Diffuse>(Vec3{albedo[0], albedo[1], albedo[2]}));
        }
        else if (type == "metal") {
            auto albedo = i["albedo"];
            auto fuzz = i["fuzz"];
            out.insert_or_assign(i["name"], makeSP<Metal>(Vec3{albedo[0], albedo[1], albedo[2]}, fuzz));
        }
        else if (type == "glass") {
            auto albedo = i["albedo"];
            auto ior = i["ior"];
            out.insert_or_assign(i["name"], makeSP<Glass>(Vec3{albedo[0], albedo[1], albedo[2]}, ior));
        }
        else if (type == "emmisive") {
            auto color = i["color"];
            auto str = i["intensity"];
            out.insert_or_assign(i["name"], makeSP<Emmisive>(Vec3{color[0], color[1], color[2]}, str));
        }
    }

    return out;
}

static HittableSP loadInternal(const std::unordered_map<std::string, MaterialSP> &mats, const json &objdata) {
    if (objdata["shape"] == "sphere") {
        auto mat = mats.at(objdata["material"]);
        auto transform = objdata["transform"];
        auto position = transform["position"];

        Vec3 center = {position[0], position[1], position[2]};
        auto radius = double(transform["radius"]);
        std::cout << "Loaded shape object " << objdata["shape"] << ".\n";
        return makeSP<Sphere>(center, radius, mat);
    }
    else {
        std::cerr << "Unkown shape directive: " << objdata["shape"] << ".\n";
        std::exit(1);
    }
}

static std::shared_ptr<HittableList> loadObject(const std::unordered_map<std::string, MaterialSP> &mats, const json &objdata) {
    if (!objdata.contains("material")) {
        std::cerr << "Object without material directive.\n";
        std::exit(1);
    }
    if (!mats.contains(objdata["material"])) {
        std::cerr << "Material not found: " << objdata["material"] << ".\n";
        std::exit(1); 
    }

    HittableSP objload;

    if (objdata.contains("model")) {
        auto mat = mats.at(objdata["material"]);
        auto filename = std::string(objdata["model"]);
        objload = loadFromObj(filename.c_str(), mat);
    }
    else if (objdata.contains("shape")) {
        auto out = makeSP<HittableList>();
        out->add(loadInternal(mats, objdata));
        return out;
    }
    else {
        std::cerr << "Object needs a loading directive (either 'model' or 'shape').\n";
        std::exit(1);
    }


    auto transform = objdata["transform"];

    auto posjs = transform["position"];
    Vec3 pos = {posjs[0], posjs[1], posjs[2]};

    Vec3 scale = {1, 1, 1};
    if (transform.contains("scale")) {
        auto scalejs = transform["scale"];
        scale = {scalejs[0], scalejs[1], scalejs[2]};
    }

    auto rotjs = transform["rotation"];
    Vec3 rotation = Vec3{rotjs[0], rotjs[1], rotjs[2]} * (mathPi / 180.0);

    auto rotMatrix = getRotationMatrix(rotation);
    auto scaleMatrix = getScaleMatrix(scale);

    auto objlist = std::dynamic_pointer_cast<HittableList>(objload);

    for (auto &i : objlist->objects) {
        auto tri = std::dynamic_pointer_cast<Triangle>(i);

        tri->a = rotMatrix * (scaleMatrix * tri->a) + pos;
        tri->b = rotMatrix * (scaleMatrix * tri->b) + pos;
        tri->c = rotMatrix * (scaleMatrix * tri->c) + pos;
        
        tri->recalculateBBox();
    }

    std::cout << "Loaded " << objlist->objects.size() << " triangles from file " << objdata["model"] << ".\n";
    return objlist;
}

std::shared_ptr<Hittable> loadScene(const char *filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open scene file: \"" << filename << "\".\n";
        std::exit(1);
    }

    json data = json::parse(file);
    auto mats = loadMats(data);

    auto out = makeSP<HittableList>();

    for (const auto &objd : data["objects"]) {
        auto obj = loadObject(mats, objd);
        out->add(obj);
    }
    std::cout << "Finished loading scene from file " << filename << ".\n";
    return out;
}
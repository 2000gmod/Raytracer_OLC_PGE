#include "mainscreen.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

#include "util.hpp"

MemoryStrategy memStrategy;

using json = nlohmann::json;

bool validateConfig(const json &config) {
    bool out = true;

    out &= config.contains("width") && config["width"].is_number_integer();
    out &= config.contains("height") && config["height"].is_number_integer();

    out &= config.contains("pixSize") && config["pixSize"].is_number_integer();

    out &= config.contains("renderTileSize") && config["renderTileSize"].is_number_integer();
    out &= config.contains("renderThreads") && config["renderThreads"].is_number_integer();

    out &= config.contains("memoryStrategy") && config["memoryStrategy"].is_string();
    out &= config.contains("scene") && config["scene"].is_string();

    return out;
}

int main() {
    std::ifstream file("rtconfig.json");
    if (!file.is_open()) {
        std::cerr << "Configuration file (\"rtconfig.json\") not found.\n";
        return 1;
    }

    json data;
    try {
        data = json::parse(file);
    }
    catch (json::parse_error &e) {
        std::cerr << "Exception while parsing rtconfig:\n";
        std::cerr << e.what() << "\n";
        return 1;
    }

    if (!validateConfig(data)) {
        std::cerr << "Invalid rtconfig format.\n";
        return 1; 
    }

    auto strategy = data["memoryStrategy"];
    if (strategy == "heap") memStrategy = MemoryStrategy::HEAP;
    else if (strategy == "arena") memStrategy = MemoryStrategy::ARENA;
    else memStrategy = MemoryStrategy::HEAP;


    int pixSize = data["pixSize"];
    int width = data["width"];
    int height = data["height"];

    auto threads = data["renderThreads"] > 0 ? (int) data["renderThreads"] : std::thread::hardware_concurrency();

    MainScreen screen(data["renderTileSize"], threads, data["scene"]);
    if (screen.Construct(width / pixSize, height / pixSize, pixSize, pixSize)) {
        screen.Start();
    }
    else {
        return 1;
    }
    return 0;
}
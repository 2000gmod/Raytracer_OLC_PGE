#pragma once

#include "hittable.hpp"

std::shared_ptr<Hittable> loadScene(const char *filename);
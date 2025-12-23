#pragma once

#include "mujs.hpp"

namespace engine {

struct Config {
    int width = 800;
    int height = 600;
    int fps = 60;
};

struct Engine {
    mujs::Js js;
};

Engine engine();

void destroy(Engine& window);

int run(Engine& self, const char *path);

} // namespace engine

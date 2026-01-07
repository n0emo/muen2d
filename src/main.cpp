#include <iostream>
#include <span>

#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>

#include <engine.hpp>
#include <defer.hpp>

namespace engine = muen::engine;

auto main(int argc, char **argv) -> int {
    spdlog::cfg::load_env_levels();

    auto args = std::span(argv, size_t(argc));

    if (argc != 2) {
        std::cerr << "Usage: muen <GAME_FOLDER>\n";
        return 1;
    }
    auto e = engine::create();
    defer(engine::destroy(*e));
    return engine::run(*e, args[1]);
    return 0;
}

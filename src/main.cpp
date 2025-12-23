#include <cstdio>

#include "engine.hpp"
#include "defer.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: muen <GAME_FOLDER>");
        return 1;
    }
    auto e = engine::engine();
    defer(engine::destroy(e));
    return engine::run(e, argv[1]);
}

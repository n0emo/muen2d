#include <span>

#include <fmt/format.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <engine.hpp>
#include <plugins/audio.hpp>
#include <plugins/console.hpp>
#include <plugins/graphics.hpp>
#include <plugins/math.hpp>
#include <plugins/window.hpp>

auto main(int argc, char **argv) noexcept -> int try {
    using namespace muen;

    spdlog::cfg::load_env_levels();

    auto args = std::span(argv, size_t(argc));

    if (argc != 2) {
        fmt::println(stderr, "Usage: muen <GAME_FOLDER>\n");
        return 1;
    }

    const auto engine_result = engine::create();
    if (!engine_result) {
        fmt::println("Error creating engine: {}", engine_result.error()->msg());
        if (auto loc = engine_result.error()->loc_str()) fmt::println("Originated from:\n    {}", *loc);
        return 1;
    }
    auto engine = engine_result.value();
    defer(engine::destroy(engine));

    SPDLOG_TRACE("Registering plugins");
    engine::register_plugin(*engine, plugins::console::plugin(engine->js));
    engine::register_plugin(*engine, plugins::math::plugin(engine->js));
    engine::register_plugin(*engine, plugins::window::plugin(engine->js));
    engine::register_plugin(*engine, plugins::graphics::plugin(engine->js));
    engine::register_plugin(*engine, plugins::audio::plugin(engine->js));

    const auto run_result = engine::run(*engine, args[1]);
    if (!run_result) {
        fmt::println(stderr, "Error running game: {}", run_result.error()->msg());
        if (auto loc = run_result.error()->loc_str()) fmt::println("Originated from:\n    {}", *loc);
        return 1;
    }

    return 0;

} catch (std::exception& e) {
    // NOLINTBEGIN: fmt::println throws exception
    fprintf(stderr, "Unexpected error: %s\n", e.what());
    fprintf(stderr, "Please file an issue at https://github.com/n0emo/muen2d/issues/new\n");
    // NOLINTEND
}

#include <span>
#include <filesystem>

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
#include <file_store.hpp>

auto main(int argc, char **argv) noexcept -> int try {
    using namespace muen;

    spdlog::cfg::load_env_levels();

    auto args = std::span(argv, size_t(argc));

    auto path_str = args[0];
    if (argc == 2) {
        path_str = argv[1];
    }

    const auto path = std::filesystem::path(path_str);

    auto engine_result = Engine::create(path);
    if (!engine_result) {
        fmt::println("Error creating engine: {}", engine_result.error()->msg());
        if (auto loc = engine_result.error()->loc_str()) fmt::println("Originated from:\n    {}", *loc);
        return 1;
    }
    auto engine = std::move(*engine_result);

    SPDLOG_TRACE("Registering plugins");
    engine->register_plugin(plugins::console::plugin(engine->js_context()));
    engine->register_plugin(plugins::math::plugin(engine->js_context()));
    engine->register_plugin(plugins::window::plugin(engine->js_context()));
    engine->register_plugin(plugins::graphics::plugin(engine->js_context()));
    engine->register_plugin(plugins::audio::plugin(engine->js_context()));

    if (auto r = engine->load_plugins(); !r) {
        fmt::println("Error loading plugins: {}", r.error()->msg());
        if (auto loc = r.error()->loc_str()) fmt::println("Originated from:\n    {}", *loc);
        return 1;
    }

    auto game_result = Game::create(engine->js_context(), &engine->file_store());
    if (!game_result) {
        fmt::println("Error creating game: {}", game_result.error()->msg());
        if (auto loc = game_result.error()->loc_str()) fmt::println("Originated from:\n    {}", *loc);
        return 1;
    }
    auto game = std::move(*game_result);

    const auto run_result = engine->run_game(game);
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

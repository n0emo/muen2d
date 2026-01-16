#include "engine.hpp"

#include <fstream>
#include <sstream>

#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <engine/game.hpp>
#include <engine/window.hpp>
#include <utility>

namespace muen::engine {

static auto module_loader(JSContext *ctx, const char *module_name, void *opaque) -> JSModuleDef *;

auto create() noexcept -> std::expected<Engine *, error::Error> {
    SPDLOG_TRACE("Creating JS runtime");
    auto runtime = JS_NewRuntime();
    if (runtime == nullptr) {
        return Err(error::create("Could not allocate runtime"));
    }

    SPDLOG_TRACE("Creating JS context");
    auto js = JS_NewContext(runtime);
    if (js == nullptr) {
        JS_FreeRuntime(runtime);
        return Err(error::create("Could not allocate context"));
    }

    SPDLOG_TRACE("Creating engine");
    auto e = new (std::nothrow) Engine {.runtime = runtime, .js = js};
    if (e == nullptr) {
        JS_FreeContext(js);
        JS_FreeRuntime(runtime);
        return Err(error::create("Could not allocate engine"));
    }

    JS_SetContextOpaque(js, e);
    JS_SetModuleLoaderFunc(runtime, nullptr, module_loader, e);

    return e;
}

auto destroy(Engine *self) noexcept -> void {
    if (!self) return;

    if (self->js) {
        SPDLOG_TRACE("Destroying JS context");
        JS_FreeContext(self->js);
    }

    if (self->runtime) {
        SPDLOG_TRACE("Destroying JS runtime");
        JS_FreeRuntime(self->runtime);
    }

    delete self;
}

auto run(Engine& self, std::string path) -> std::expected<std::monostate, error::Error> {
    SPDLOG_DEBUG("Running engine with game {}", path);
    window::setup();

    self.root_path = std::move(path);
    if (self.root_path[self.root_path.size() - 1] != '/') {
        self.root_path += '/';
    }

    SPDLOG_DEBUG("Loading plugins");
    for (const auto& callback : self.load_callbacks) {
        if (auto result = callback(); !result) return Err(std::move(result.error()));
    }

    defer({
        SPDLOG_TRACE("Unloading plugins");
        for (const auto& callback : self.unload_callbacks) {
            if (auto result = callback(); !result) {
                SPDLOG_WARN("Error unloading plugin: {}", result.error()->msg());
            }
        }
    });

    SPDLOG_DEBUG("Loading game");
    auto game = game::Game {};
    if (auto game_result = game::create(self.js, self.root_path); game_result) game = *game_result;
    else return Err(game_result.error());
    defer({
        SPDLOG_TRACE("Destroying game");
        game::destroy(game);
    });

    SPDLOG_DEBUG("Creating window");
    auto w = window::create(
        window::Config {
            .width = game.config.width,
            .height = game.config.height,
            .fps = game.config.fps,
            .title = game.config.title,
        }
    );
    defer({
        SPDLOG_TRACE("Closing window");
        window::close(w);
    });

    SPDLOG_DEBUG("Loading game");
    if (auto r = game::load(game); !r) return Err(r.error());

    SPDLOG_DEBUG("Running rame");
    while (!window::should_close(w)) {
        SPDLOG_TRACE("Updating plugins");
        for (const auto& callback : self.update_callbacks) {
            if (auto r = callback(); !r) return Err(r.error());
        }

        if (IsKeyPressed(KEY_F5)) {
            auto state = game::pre_reload(game);
            if (!state) {
                SPDLOG_ERROR("Exception occured while pre-reloading game: {}", state.error()->msg());
                continue;
            }
            defer(JS_FreeValue(self.js, *state));

            auto game_result = game::create(self.js, self.root_path);
            if (!game_result) {
                SPDLOG_ERROR("Exception occured while reloading the game: {}", game_result.error()->msg());
                continue;
            }

            if (auto r = game::load(*game_result); !r) {
                SPDLOG_ERROR("Exception occured while calling game load after reloading: {}", r.error()->msg());
                continue;
            }

            if (auto r = game::post_reload(*game_result, *state); !r) {
                SPDLOG_ERROR("Exception occured while post-reloading game: {}", r.error()->msg());
                continue;
            }

            game::destroy(game);
            game = *game_result;
        }

        SPDLOG_TRACE("Updating game");
        if (auto result = game::update(game); !result) return Err(result.error());

        window::begin_drawing(w);

        SPDLOG_TRACE("Drawing plugins");
        for (const auto& callback : self.draw_callbacks) {
            if (auto result = callback(); !result) return Err(result.error());
        }

        SPDLOG_TRACE("Drawing game");
        if (auto result = game::draw(game); !result) return Err(result.error());

        window::draw_fps(w);
        window::end_drawing(w);
    }

    return {};
}

auto register_plugin(Engine& self, const plugins::EnginePlugin& desc) -> void {
    for (const auto& [name, module] : desc.c_modules) {
        // TODO: check if already exists
        self.c_modules.insert({name, module});
    }

    for (const auto& [name, module] : desc.js_modules) {
        // TODO: check if already exists
        self.js_modules.insert({name, module});
    }

    if (desc.load != nullptr) {
        self.load_callbacks.emplace_back(desc.load);
    }

    if (desc.unload != nullptr) {
        self.unload_callbacks.emplace_back(desc.unload);
    }

    if (desc.update != nullptr) {
        self.update_callbacks.emplace_back(desc.update);
    }

    if (desc.draw != nullptr) {
        self.draw_callbacks.emplace_back(desc.draw);
    }

    SPDLOG_INFO("Registered `{}` plugin", desc.name);
}

auto from_js(JSContext *js) -> Engine& {
    auto ptr = static_cast<Engine *>(JS_GetContextOpaque(js));
    assert(ptr);
    return *ptr;
}

auto resolve_path(const Engine& self, const std::string& path) -> std::string {
    if (path.starts_with("/")) {
        return path;
    }
    return self.root_path + path;
}

static auto module_loader(JSContext *ctx, const char *module_name, void *opaque) -> JSModuleDef * {
    SPDLOG_DEBUG("Loading module {}", module_name);

    auto e = static_cast<Engine *>(opaque);

    if (auto cm = e->c_modules.find(module_name); cm != e->c_modules.end()) {
        SPDLOG_DEBUG("Module {} resolved as builtin native module", module_name);
        return cm->second;
    } else {
        auto code = std::string {};
        if (auto jm = e->js_modules.find(module_name); jm != e->js_modules.end()) {
            SPDLOG_DEBUG("Module {} resolved as builtin js module", module_name);
            code = jm->second;
        } else {
            // TODO: error handling
            auto path = resolve_path(*e, module_name);
            if (!path.ends_with(".js")) {
                path += ".js";
            }
            SPDLOG_TRACE("Loading module {}", path);
            auto file = std::ifstream {path};
            auto buf = std::stringstream {};
            buf << file.rdbuf();
            SPDLOG_DEBUG("Module {} resolved as game module", module_name);
            code = buf.str();
        }

        JSValue ret =
            JS_Eval(ctx, code.c_str(), code.size(), module_name, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
        if (JS_IsException(ret)) {
            return nullptr;
        }
        JS_FreeValue(ctx, ret);

        return static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(ret));
    }
}

} // namespace muen::engine

#include "./engine/audio.cpp"
#include "./engine/game.cpp"
#include "./engine/music.cpp"
#include "./engine/sound.cpp"
#include "./engine/window.cpp"

#include "engine.hpp"

#include <fstream>
#include <sstream>

#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <engine/game.hpp>
#include <engine/window.hpp>
#include <plugins/audio.hpp>
#include <plugins/console.hpp>
#include <plugins/graphics.hpp>
#include <plugins/math.hpp>
#include <plugins/window.hpp>

namespace muen::engine {

void report_error(JSContext *js, const std::string& message, JSValue e) {
    defer(JS_FreeValue(js, e));

    const char *msg = JS_ToCString(js, e);
    assert(msg != nullptr);
    defer(JS_FreeCString(js, msg));

    const auto stack = JS_GetPropertyStr(js, e, "stack");
    defer(JS_FreeValue(js, stack));
    const auto stack_str = JS_ToCString(js, stack);
    assert(stack_str != nullptr);
    defer(JS_FreeCString(js, stack_str));

    spdlog::error("{}: {}\n{}", message, msg, stack_str);
}

void report_error(JSContext *js, const std::string& message) {
    const auto e = JS_GetException(js);
    report_error(js, message, e);
}

auto create() -> Engine * {
    SPDLOG_TRACE("Creating JS runtime");
    auto runtime = JS_NewRuntime();
    assert(runtime != nullptr);

    SPDLOG_TRACE("Creating JS context");
    auto js = JS_NewContext(runtime);
    assert(js != nullptr);

    SPDLOG_TRACE("Creating engine");
    auto e = new Engine {.runtime = runtime, .js = js};
    assert(e != nullptr);
    JS_SetContextOpaque(js, e);

    return e;
}

void destroy(Engine& self) {
    if (self.js) {
        SPDLOG_TRACE("Destroying JS context");
        JS_FreeContext(self.js);
    }

    if (self.runtime) {
        SPDLOG_TRACE("Destroying JS runtime");
        JS_FreeRuntime(self.runtime);
    }
}

auto module_loader(JSContext *ctx, const char *module_name, void *opaque) -> JSModuleDef * {
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

auto run(Engine& self, const char *path) -> int {
    SPDLOG_DEBUG("Running engine with game {}", path);

    window::setup();

    self.root_path = std::string {path};
    if (self.root_path[self.root_path.size() - 1] != '/') {
        self.root_path += '/';
    }

    SPDLOG_TRACE("Registering plugins");
    register_plugin(self, plugins::console::plugin(self.js));
    register_plugin(self, plugins::math::plugin(self.js));
    register_plugin(self, plugins::window::plugin(self.js));
    register_plugin(self, plugins::graphics::plugin(self.js));
    register_plugin(self, plugins::audio::plugin(self.js));

    JS_SetModuleLoaderFunc(self.runtime, nullptr, module_loader, &self);

    SPDLOG_DEBUG("Loading plugins");
    for (const auto& callback : self.load_callbacks) {
        callback();
        // TODO: better error handling
        if (JS_HasException(self.js)) {
            report_error(self.js, "Exception occured while loading plugins");
            return 1;
        }
    }

    defer({
        SPDLOG_TRACE("Unloading plugins");
        for (const auto& callback : self.unload_callbacks) {
            callback();
        }
    });

    SPDLOG_DEBUG("Loading game");
    auto game = game::Game {};
    if (auto game_result = game::create(self.js, self.root_path); game_result) {
        game = *game_result;
    } else {
        report_error(self.js, "Exception occured while initializing game", game_result.error());
        return 1;
    }
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
    if (auto result = game::load(game); !result.has_value()) {
        report_error(self.js, "Exception occured while initializing game");
        return 1;
    }

    SPDLOG_DEBUG("Running rame");
    while (!window::should_close(w)) {
        SPDLOG_TRACE("Updating plugins");
        for (const auto& callback : self.update_callbacks) {
            callback();
            // TODO: better error handling
            if (JS_HasException(self.js)) {
                report_error(self.js, "Exception occured while updating plugins");
                return 1;
            }
        }

        if (IsKeyPressed(KEY_F5)) {
            auto state = game::pre_reload(game);
            if (!state) {
                report_error(self.js, "Exception occured while pre-reloading game");
                continue;
            }
            defer(JS_FreeValue(self.js, *state));

            auto game_result = game::create(self.js, self.root_path);
            if (!game_result) {
                report_error(self.js, "Exception occured while reloading the game");
                continue;
            }

            if (!game::load(*game_result)) {
                report_error(self.js, "Exception occured while calling game load after reloading");
                continue;
            }

            if (auto result = game::post_reload(*game_result, *state); !result) {
                report_error(self.js, "Exception occured while post-reloading game");
                continue;
            }

            game::destroy(game);
            game = *game_result;
        }

        SPDLOG_TRACE("Updating game");
        if (auto result = game::update(game); !result.has_value()) {
            report_error(self.js, "Exception occured while updating game");
            return 1;
        }

        window::begin_drawing(w);

        SPDLOG_TRACE("Drawing plugins");
        for (const auto& callback : self.draw_callbacks) {
            callback();
            // TODO: better error handling
            if (JS_HasException(self.js)) {
                report_error(self.js, "Exception occured while updating plugins");
                return 1;
            }
        }

        SPDLOG_TRACE("Drawing game");
        if (auto result = game::draw(game); !result) {
            report_error(self.js, "Exception occured while rendering game", result.error());
            return 1;
        }

        window::draw_fps(w);
        window::end_drawing(w);
    }

    return 0;
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

} // namespace muen::engine

#include "./engine/audio.cpp"
#include "./engine/game.cpp"
#include "./engine/music.cpp"
#include "./engine/sound.cpp"
#include "./engine/window.cpp"

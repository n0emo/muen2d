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
    defer(JS_FreeCString(js, msg));

    const auto stack = JS_GetPropertyStr(js, e, "stack");
    defer(JS_FreeValue(js, stack));
    const auto stack_str = JS_ToCString(js, stack);
    defer(JS_FreeCString(js, stack_str));

    spdlog::error("{}: {}\n{}", message, msg, stack_str);
}

void report_error(JSContext *js, const std::string& message) {
    const auto e = JS_GetException(js);
    report_error(js, message, e);
}

auto create() -> Engine {
    auto runtime = JS_NewRuntime();
    auto js = JS_NewContext(runtime);
    auto e = Engine {.runtime = runtime, .js = js};
    JS_SetContextOpaque(js, &e);
    return e;
}

void destroy(Engine& self) {
    JS_FreeContext(self.js);
    JS_FreeRuntime(self.runtime);
}

auto module_loader(JSContext *ctx, const char *module_name, void *opaque) -> JSModuleDef * {
    auto e = static_cast<Engine *>(opaque);

    if (auto cm = e->c_modules.find(module_name); cm != e->c_modules.end()) {
        return cm->second;
    } else {
        auto code = std::string {};
        if (auto jm = e->js_modules.find(module_name); jm != e->js_modules.end()) {
            code = jm->second;
        } else {
            // TODO: error handling
            auto path = resolve_path(*e, module_name);
            auto file = std::ifstream {path};
            auto buf = std::stringstream {};
            buf << file.rdbuf();
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
    window::setup();

    self.root_path = std::string {path};
    if (self.root_path[self.root_path.size() - 1] != '/') {
        self.root_path += '/';
    }

    register_plugin(self, plugins::console::plugin(self.js));
    register_plugin(self, plugins::math::plugin(self.js));
    register_plugin(self, plugins::window::plugin(self.js));
    register_plugin(self, plugins::graphics::plugin(self.js));
    register_plugin(self, plugins::audio::plugin(self.js));

    JS_SetModuleLoaderFunc(self.runtime, nullptr, module_loader, &self);

    for (const auto& callback : self.load_callbacks) {
        callback();
        // TODO: better error handling
        if (JS_HasException(self.js)) {
            report_error(self.js, "Exception occured while loading plugins");
            return 1;
        }
    }

    defer({
        for (const auto& callback : self.unload_callbacks) {
            callback();
        }
    });

    const auto game_result = game::create(self.js, self.root_path);
    if (!game_result.has_value()) {
        report_error(self.js, "Exception occured while initializing game", game_result.error());
        return 1;
    }
    auto game = *game_result;
    defer(game::destroy(game));

    auto w = window::create(
        window::Config {
            .width = game.config.width,
            .height = game.config.height,
            .fps = game.config.fps,
            .title = game.config.title,
        }
    );
    defer(window::close(w));

    // auto a = audio::create();
    // defer(audio::close(a));

    if (auto result = game::load(game); !result.has_value()) {
        report_error(self.js, "Exception occured while initializing game");
        return 1;
    }

    while (!window::should_close(w)) {
        for (const auto& callback : self.update_callbacks) {
            callback();
            // TODO: better error handling
            if (JS_HasException(self.js)) {
                report_error(self.js, "Exception occured while updating plugins");
                return 1;
            }
        }

        if (auto result = game::update(game); !result.has_value()) {
            report_error(self.js, "Exception occured while updating game");
            return 1;
        }

        window::begin_drawing(w);

        for (const auto& callback : self.draw_callbacks) {
            callback();
            // TODO: better error handling
            if (JS_HasException(self.js)) {
                report_error(self.js, "Exception occured while updating plugins");
                return 1;
            }
        }

        if (auto result = game::draw(game); !result.has_value()) {
            report_error(self.js, "Exception occured while rendering game");
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

    spdlog::info("Registered `{}` plugin", desc.name);
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

#include "engine.hpp"

#include <spdlog/spdlog.h>

#include "js.hpp"
#include "bindings.hpp"
#include "defer.hpp"
#include "window.hpp"

namespace muen::engine {

constexpr char CAMERA_JS[] = {
#include "Camera.js.h" 
};
constexpr char COLOR_JS[] = {
#include "Color.js.h" 
};
constexpr char CONSOLE_JS[] = {
#include "Console.js.h" 
};
constexpr char GRAPHICS_JS[] = {
#include "graphics.js.h" 
};
constexpr char RECTANGLE_JS[] = {
#include "Rectangle.js.h" 
};
constexpr char SCREEN_JS[] = {
#include "screen.js.h" 
};
constexpr char VECTOR2_JS[] = {
#include "Vector2.js.h" 
};

auto read_config(Engine& self) -> Config;

auto create() -> Engine {
    auto js = js::newstate(nullptr, nullptr, js::STRICT);
    return Engine {.js = js};
}

void destroy(Engine& self) {
    js::freestate(self.js);
}

auto run(Engine& self, const char *path) -> int {
    window::setup();

    self.root_path = path;
    self.muen_modules = {
        {"muen/Camera.js", CAMERA_JS},
        {"muen/Color.js", COLOR_JS},
        {"muen/Console.js", CONSOLE_JS},
        {"muen/Rectangle.js", RECTANGLE_JS},
        {"muen/Vector2.js", VECTOR2_JS},
        {"muen/graphics.js", GRAPHICS_JS},
        {"muen/screen.js", SCREEN_JS},
    };

    js::setcontext(self.js, &self);

    bindings::define(self.js);

    auto game_path = std::string {path};
    if (game_path[game_path.size() - 1] != '/') {
        game_path += '/';
    }
    game_path += "Game.js";

    Config config;
    try {
        mujs_catch(self.js);

        js::pushstring(
            self.js,
            R"(
            var Game = require("Game");
            var game = new Game();
            )"
        );
        js::eval(self.js);
        js::pop(self.js, 1);
        js::endtry(self.js);

        config = read_config(self);
    } catch (const js::Exception& e) {
        spdlog::error("Error initializing game: {}", e.what());
        return 1;
    }

    auto w = window::create(
        window::Config {
            .width = config.width,
            .height = config.height,
            .fps = config.fps,
            .title = config.title,
        }
    );
    defer(window::close(w));

    try {
        mujs_catch(self.js);
        while (!window::should_close(w)) {
            js::getglobal(self.js, "game");
            js::getproperty(self.js, -1, "update");
            js::rot2(self.js);
            js::call(self.js, 0);
            js::pop(self.js, 1);

            window::begin_drawing(w);

            js::getglobal(self.js, "game");
            js::getproperty(self.js, -1, "draw");
            js::rot2(self.js);
            js::call(self.js, 0);
            js::pop(self.js, 1);

            window::draw_fps(w);
            window::end_drawing(w);
        }
        js::endtry(self.js);
    } catch (js::Exception& e) {
        spdlog::error("Error running game: {}\n", e.what());
        js::dump(self.js);
        return 1;
    }

    return 0;
}

auto read_config(Engine& self) -> Config {
    mujs_catch(self.js);

    auto config = Config {};
    js::getglobal(self.js, "game");
    if (js::hasproperty(self.js, -1, "config")) {
        if (js::hasproperty(self.js, -1, "fps")) {
            config.fps = js::tointeger(self.js, -1);
            js::pop(self.js, 1);
        }

        if (js::hasproperty(self.js, -1, "width")) {
            config.width = js::tointeger(self.js, -1);
            js::pop(self.js, 1);
        }

        if (js::hasproperty(self.js, -1, "height")) {
            config.height = js::tointeger(self.js, -1);
            js::pop(self.js, 1);
        }

        if (js::hasproperty(self.js, -1, "title")) {
            config.title = js::tostring(self.js, -1);
            js::pop(self.js, 1);
        }

        js::pop(self.js, 1);
    }
    js::pop(self.js, 1);

    js::endtry(self.js);

    return config;
}

} // namespace muen::engine

#include "engine.hpp"

#include <spdlog/spdlog.h>

#include "defer.hpp"
#include "jsutil.h"
#include "objects.hpp"
#include "window.hpp"

namespace engine {

Config read_config(Engine& self);

Engine engine() {
    auto js = mujs::Js::create();
    return Engine {.js = js};
}

void destroy(Engine& self) {
    self.js.destroy();
}

int run(Engine& self, const char *path) {
    objects::define(self.js, path);

    auto game_path = std::string {path};
    if (game_path[game_path.size() - 1] != '/') {
        game_path += '/';
    }
    game_path += "Game.js";

    try {
        self.js.eval_file(game_path.c_str());
        self.js.eval_string("var game = new Game();");
        self.js.pop(1);
    } catch (const mujs::Exception& e) {
        spdlog::error("Error initializing game: {}", e.what());
        return 1;
    }

    auto config = read_config(self);

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
        while (!window::should_close(w)) {
            self.js.eval_string("game.update()");
            self.js.pop(1);
            window::begin_drawing(w);
            self.js.eval_string("game.draw()");
            self.js.pop(1);
            window::draw_fps(w);
            window::end_drawing(w);
        }
    } catch (mujs::Exception& e) {
        spdlog::error("Error running game: {}", e.what());
        printf("\n");
        ::js_dump(self.js.j);
        return 1;
    }

    return 0;
}

Config read_config(Engine& self) {
    auto config = Config {};
    auto game_obj = self.js.get_global_object("game");
    if (auto config_obj = game_obj.get_object("config")) {
        if (auto fps = config_obj->get_integer("fps")) {
            config.fps = *fps;
        }
        if (auto width = config_obj->get_integer("width")) {
            config.width = *width;
        }
        if (auto height = config_obj->get_integer("height")) {
            config.height = *height;
        }
        if (auto title = config_obj->get_string("title")) {
            config.title = *title;
        }
        config_obj->drop();
    }
    game_obj.drop();

    return config;
}

} // namespace engine

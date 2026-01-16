#pragma once

#include <quickjs.h>

#include <string>

#include <error.hpp>

namespace muen::game {

struct Config {
    int width = 800;
    int height = 600;
    int fps = 60;
    std::string title = "muen2d";
};

struct Game {
    JSContext *js;
    Config config;
    JSValue mod;
    JSValue load;
    JSValue update;
    JSValue draw;
    JSValue pre_reload;
    JSValue post_reload;
};

auto create(JSContext *js, std::string path) -> Result<Game>;
auto destroy(Game& self) -> void;
auto load(Game& self) -> Result<>;
auto update(Game& self) -> Result<>;
auto draw(Game& self) -> Result<>;
auto pre_reload(Game& self) -> Result<JSValue>;
auto post_reload(Game& self, JSValue state) -> Result<>;

} // namespace muen::game

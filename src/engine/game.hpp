#pragma once

#include <quickjs.h>

#include <string>
#include <expected>
#include <variant>

namespace muen::game {

struct Config {
    int width = 800;
    int height = 600;
    int fps = 60;
    const char *title = "muen2d";
};

struct Game {
    JSContext *js;
    Config config;
    JSValue mod;
    JSValue load;
    JSValue update;
    JSValue draw;
};

auto create(JSContext *js, std::string path) -> std::expected<Game, JSValue>;
auto destroy(Game& self) -> void;
auto load(Game& self) -> std::expected<std::monostate, JSValue>;
auto update(Game& self) -> std::expected<std::monostate, JSValue>;
auto draw(Game& self) -> std::expected<std::monostate, JSValue>;

} // namespace muen::game

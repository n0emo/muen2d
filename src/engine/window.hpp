#pragma once

namespace window {

struct Config {
    int width;
    int height;
    int fps;
    const char *title;
};

struct Window {
    Config config;
};

auto setup() -> void;
auto create(Config config) -> Window;
auto close(Window& self) -> void;
auto should_close(Window& self) -> bool;
auto begin_drawing(Window& self) -> void;
auto end_drawing(Window& self) -> void;
auto clear(Window& self) -> void;
auto draw_fps(Window& self) -> void;

} // namespace window

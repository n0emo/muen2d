#pragma once

#include <string>

namespace window {

struct Config {
    int width;
    int height;
    int fps;
    std::string title;
};

struct Window {
    Config config;
};

auto setup() -> void;
auto create(const Config& config) -> Window;
auto close(Window& self) -> void;
auto should_close(Window& self) -> bool;
auto begin_drawing(Window& self) -> void;
auto end_drawing(Window& self) -> void;
auto clear(Window& self) -> void;
auto draw_fps(Window& self) -> void;

} // namespace window

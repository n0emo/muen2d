#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <expected>

#include <raylib.h>

#include "js.hpp"
#include "audio.hpp"

namespace muen::engine {

struct Config {
    int width = 800;
    int height = 600;
    int fps = 60;
    const char *title = "muen2d";
};

struct Engine {
    js::State *js {};
    const char *root_path {};
    std::unordered_map<std::string, std::string> muen_modules {};
    std::unordered_map<uint32_t, audio::Sound> sounds {};
    std::unordered_map<std::string, uint32_t> sounds_cache {};
    std::unordered_map<uint32_t, audio::Music> musics {};
    std::unordered_map<std::string, uint32_t> musics_cache {};
};

auto create() -> Engine;
auto destroy(Engine& window) -> void;
auto run(Engine& self, const char *path) -> int;

auto is_audio_ready(Engine& self) -> bool;

auto load_sound(Engine& self, std::string path) -> std::expected<uint32_t, std::string>;
auto unload_sound(Engine& self, uint32_t id) -> void;
auto get_sound(Engine& self, uint32_t id) -> std::optional<audio::Sound>;

auto load_music(Engine& self, const std::string& filename) -> std::expected<uint32_t, std::string>;
auto unload_music(Engine& self, uint32_t id) -> void;
auto get_music(Engine& self, uint32_t id) -> std::optional<audio::Music>;

} // namespace muen::engine

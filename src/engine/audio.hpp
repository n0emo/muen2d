#pragma once

#include <set>
#include <expected>
#include <string>

#include <raylib.h>

namespace muen::engine::audio {

namespace music {
    struct Music {
        ::Music music {};
        float volume = 1.0f;
        float pitch = 1.0f;
        float pan = 0.5f;
    };

    auto load(const std::string& path) -> std::expected<Music *, std::string>;
    auto unload(Music *self) -> void;
    auto update(Music& self) -> void;
    auto play(Music& self) -> void;
    auto stop(Music& self) -> void;
    auto pause(Music& self) -> void;
    auto resume(Music& self) -> void;
    auto seek(Music& self, float cursor) -> void;
    auto is_playing(const Music& self) -> bool;
    auto get_looping(const Music& self) -> bool;
    auto set_looping(Music& self, bool looping) -> void;
    auto get_volume(const Music& self) -> float;
    auto set_volume(Music& self, float volume) -> void;
    auto get_pan(const Music& self) -> float;
    auto set_pan(Music& self, float pan) -> void;
    auto get_pitch(const Music& self) -> float;
    auto set_pitch(Music& self, float pitch) -> void;
} // namespace music

namespace sound {
    struct Sound {
        ::Sound sound {};
        float volume = 1.0f;
        float pitch = 1.0f;
        float pan = 0.5f;
    };

    auto load(const std::string& path) -> std::expected<Sound *, std::string>;
    auto unload(Sound *self) -> void;
    auto play(Sound& self) -> void;
    auto stop(Sound& self) -> void;
    auto pause(Sound& self) -> void;
    auto resume(Sound& self) -> void;
    auto is_playing(Sound& self) -> bool;
    auto get_volume(Sound& self) -> float;
    auto set_volume(Sound& self, float volume) -> void;
    auto get_pan(Sound& self) -> float;
    auto set_pan(Sound& self, float pan) -> void;
    auto get_pitch(Sound& self) -> float;
    auto set_pitch(Sound& self, float pitch) -> void;
} // namespace sound

using Music = music::Music;
using Sound = sound::Sound;

struct Audio {
    std::set<Music *> musics {};
    std::set<Sound *> sounds {};
};

auto init() -> void;
auto close() -> void;
auto get() -> Audio&;

} // namespace muen::engine::audio

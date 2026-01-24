#pragma once

#include <filesystem>
#include <gsl/gsl>
#include <set>

#include <error.hpp>
#include <file_store.hpp>
#include <raylib.hpp>

namespace muen::engine::audio {

using namespace gsl;

namespace music {
    struct Music {
        rl::Music music {};
        std::unique_ptr<unsigned char[]> data;
        float volume = 1.0f;
        float pitch = 1.0f;
        float pan = 0.5f;
    };

    auto load(const std::filesystem::path& name, IFileStore& store) noexcept -> Result<Music>;
    auto update(Music& self) noexcept -> void;
    auto play(Music& self) noexcept -> void;
    auto stop(Music& self) noexcept -> void;
    auto pause(Music& self) noexcept -> void;
    auto resume(Music& self) noexcept -> void;
    auto seek(Music& self, float cursor) noexcept -> void;
    auto is_playing(const Music& self) noexcept -> bool;
    auto get_looping(const Music& self) noexcept -> bool;
    auto set_looping(Music& self, bool looping) noexcept -> void;
    auto get_volume(const Music& self) noexcept -> float;
    auto set_volume(Music& self, float volume) noexcept -> void;
    auto get_pan(const Music& self) noexcept -> float;
    auto set_pan(Music& self, float pan) noexcept -> void;
    auto get_pitch(const Music& self) noexcept -> float;
    auto set_pitch(Music& self, float pitch) noexcept -> void;
} // namespace music

namespace sound {
    struct Sound {
        rl::Sound sound {};
        float volume = 1.0f;
        float pitch = 1.0f;
        float pan = 0.5f;
    };

    auto load(const std::filesystem::path& name, IFileStore& store) noexcept -> Result<Sound>;
    auto play(Sound& self) noexcept -> void;
    auto stop(Sound& self) noexcept -> void;
    auto pause(Sound& self) noexcept -> void;
    auto resume(Sound& self) noexcept -> void;
    auto is_playing(Sound& self) noexcept -> bool;
    auto get_volume(Sound& self) noexcept -> float;
    auto set_volume(Sound& self, float volume) noexcept -> void;
    auto get_pan(Sound& self) noexcept -> float;
    auto set_pan(Sound& self, float pan) noexcept -> void;
    auto get_pitch(Sound& self) noexcept -> float;
    auto set_pitch(Sound& self, float pitch) noexcept -> void;
} // namespace sound

using Music = music::Music;
using Sound = sound::Sound;

struct Audio {
    std::set<Music *> musics {};
    std::set<Sound *> sounds {};
};

auto init() noexcept -> void;
auto close() noexcept -> void;
auto get() noexcept -> Audio&;

} // namespace muen::engine::audio

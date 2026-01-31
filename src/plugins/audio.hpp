#pragma once

#include <gsl/gsl>

#include <engine/audio.hpp>
#include <engine/plugin.hpp>
#include <quickjs.hpp>
#include <raylib.hpp>

namespace glint::js {

template<>
auto convert_from_js<engine::audio::Music *>(const Value& val) noexcept -> JSResult<engine::audio::Music *>;

template<>
auto convert_from_js<engine::audio::Sound *>(const Value& val) noexcept -> JSResult<engine::audio::Sound *>;

} // namespace glint::js

namespace glint::plugins::audio {

using namespace gsl;

auto plugin(JSContext *js) -> EnginePlugin;

namespace music_class {
    using Music = engine::audio::Music;

    extern const JSClassDef MUSIC;
    auto module(JSContext *js) -> JSModuleDef *;
} // namespace music_class

namespace sound_class {
    using Sound = engine::audio::Sound;

    extern const JSClassDef SOUND;
    auto module(JSContext *js) -> ::JSModuleDef *;
} // namespace sound_class

} // namespace glint::plugins::audio

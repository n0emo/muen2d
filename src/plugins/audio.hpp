#pragma once

#include <gsl/gsl>

#include <engine/audio.hpp>
#include <engine/plugin.hpp>
#include <quickjs.hpp>
#include <raylib.hpp>

namespace muen::js {

template<>
auto try_into<engine::audio::Music *>(const Value& val) noexcept -> JSResult<engine::audio::Music *>;

template<>
auto try_into<engine::audio::Sound *>(const Value& val) noexcept -> JSResult<engine::audio::Sound *>;

} // namespace muen::js

namespace muen::plugins::audio {

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

} // namespace muen::plugins::audio

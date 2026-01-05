#pragma once

#include <engine/audio.hpp>
#include <engine/plugin.hpp>

namespace muen::plugins::audio {

auto plugin(JSContext *js) -> EnginePlugin;

namespace music_class {
    auto module(JSContext *js) -> JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto from_value_unsafe(JSContext *js, JSValueConst val) -> engine::audio::Music *;
} // namespace music_class

namespace sound_class {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto sound_class_id(JSContext *js) -> ::JSClassID;
    auto from_value_unsafe(::JSContext *js, ::JSValueConst val) -> engine::audio::Sound *;
} // namespace sound_class

} // namespace muen::plugins::audio

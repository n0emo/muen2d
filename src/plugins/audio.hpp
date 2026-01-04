#pragma once

#include <engine/plugin.hpp>

namespace muen::plugins::audio {

auto plugin(JSContext *js) -> EnginePlugin;
auto music_class_id(JSContext *js) -> ::JSClassID;
auto sound_class_id(JSContext *js) -> ::JSClassID;

} // namespace muen::plugins::audio

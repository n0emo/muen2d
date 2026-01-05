#pragma once

#include <engine/plugin.hpp>

namespace muen::plugins::window {

auto screen_module(::JSContext *js) -> ::JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

} // namespace muen::plugins::window

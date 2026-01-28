#pragma once

#include <engine/plugin.hpp>

namespace glint::plugins::console {

auto module(JSContext *js) -> JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

} // namespace glint::plugins::console

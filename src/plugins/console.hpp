#pragma once

#include <engine/plugin.hpp>

namespace muen::plugins::console {

auto module(JSContext *js) -> JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

} // namespace muen::plugins::console

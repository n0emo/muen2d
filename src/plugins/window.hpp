#pragma once

#include <engine/plugin.hpp>

namespace muen::plugins::window {

auto screen_module(::JSContext *js) -> ::JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

namespace mouse {
    auto module(::JSContext *js) -> ::JSModuleDef *;
}

} // namespace muen::plugins::window

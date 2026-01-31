#pragma once

#include <engine/plugin.hpp>
#include <quickjs.hpp>
#include <raylib.h>

namespace glint::js {

template<>
auto convert_from_js<MouseButton>(const Value& val) noexcept -> JSResult<MouseButton>;

template<>
auto convert_from_js<MouseCursor>(const Value& val) noexcept -> JSResult<MouseCursor>;

} // namespace glint::js

namespace glint::plugins::window {

auto screen_module(::JSContext *js) -> ::JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

namespace mouse {
    auto module(::JSContext *js) -> ::JSModuleDef *;
}

} // namespace glint::plugins::window

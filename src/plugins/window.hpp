#pragma once

#include <engine/plugin.hpp>
#include <quickjs.hpp>
#include <raylib.h>

namespace muen::js {

template<>
auto try_into<MouseButton>(const Value& val) noexcept -> JSResult<MouseButton>;

template<>
auto try_into<MouseCursor>(const Value& val) noexcept -> JSResult<MouseCursor>;

} // namespace muen::js

namespace muen::plugins::window {

auto screen_module(::JSContext *js) -> ::JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

namespace mouse {
    auto module(::JSContext *js) -> ::JSModuleDef *;
}

} // namespace muen::plugins::window

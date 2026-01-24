#pragma once

#include <spdlog/spdlog.h>

#include <engine/plugin.hpp>
#include <quickjs.hpp>
#include <raylib.hpp>

namespace muen::plugins::math {

auto plugin(JSContext *js) -> EnginePlugin;

namespace vector2 {
    extern const JSClassDef VECTOR2;
    auto module(JSContext *js) -> ::JSModuleDef *;
    auto to_string(Vector2 vec) -> std::string;
} // namespace vector2

namespace rectangle {
    extern const JSClassDef RECTANGLE;
    auto module(JSContext *js) -> ::JSModuleDef *;
    auto to_string(Rectangle rec) -> std::string;
} // namespace rectangle

} // namespace muen::plugins::math

namespace muen::js {

template<>
auto try_into<Vector2>(const Value& v) noexcept -> JSResult<Vector2>;

template<>
auto try_into<Rectangle>(const Value& v) noexcept -> JSResult<Rectangle>;

} // namespace muen::js

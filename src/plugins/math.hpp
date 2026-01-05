#pragma once

#include <expected>

#include <raylib.h>

#include <engine/plugin.hpp>

namespace muen::plugins::math {

auto plugin(JSContext *js) -> EnginePlugin;

namespace vector2 {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Vector2, ::JSValue>;
} // namespace vector2

namespace rectangle {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Rectangle, ::JSValue>;
} // namespace rectangle

} // namespace muen::plugins::math

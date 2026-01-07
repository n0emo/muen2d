#pragma once

#include <expected>

#include <raylib.h>

#include <engine/plugin.hpp>
#include <jsutil.hpp>

namespace muen::plugins::math {

auto plugin(JSContext *js) -> EnginePlugin;

namespace vector2 {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Vector2, ::JSValue>;
    auto to_string(::Vector2 vec) -> std::string;
} // namespace vector2

namespace rectangle {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Rectangle, ::JSValue>;
} // namespace rectangle

} // namespace muen::plugins::math

namespace js {

template<>
auto try_as<::Vector2>(::JSContext *js, ::JSValueConst value) -> std::expected<::Vector2, ::JSValue>;

template<>
auto try_as<::Rectangle>(::JSContext *js, ::JSValueConst value) -> std::expected<::Rectangle, ::JSValue>;

} // namespace js

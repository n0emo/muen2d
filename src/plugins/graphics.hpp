#pragma once

#include <expected>

#include <raylib.h>

#include <engine/plugin.hpp>
#include <jsutil.hpp>

namespace muen::plugins::graphics {

auto module(JSContext *js) -> JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

namespace color {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Color, ::JSValue>;
    auto to_string(Color color) -> std::string;
} // namespace color

namespace camera {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Camera2D, ::JSValue>;
} // namespace camera

namespace texture {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Texture, ::JSValue>;
} // namespace texture

namespace npatch {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::NPatchInfo, ::JSValue>;
} // namespace npatch

namespace font {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value_unsafe(::JSContext *js, ::JSValueConst val) -> ::Font *;
} // namespace font

} // namespace muen::plugins::graphics

namespace js {

template<>
auto try_as<::Color>(::JSContext *js, ::JSValueConst value) -> std::expected<::Color, ::JSValue>;

template<>
auto try_as<::Font>(::JSContext *js, ::JSValueConst value) -> std::expected<::Font, ::JSValue>;

} // namespace js

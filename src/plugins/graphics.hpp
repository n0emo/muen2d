#pragma once

#include <expected>

#include <raylib.h>

#include <engine/plugin.hpp>

namespace muen::plugins::graphics {

auto module(JSContext *js) -> JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

namespace color {
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto class_id(JSContext *js) -> ::JSClassID;
    auto class_id(JSRuntime *rt) -> ::JSClassID;
    auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Color, ::JSValue>;
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

} // namespace muen::plugins::graphics

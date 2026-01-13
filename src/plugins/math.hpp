#pragma once

#include <format>
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
    auto to_string(Rectangle rec) -> std::string;
} // namespace rectangle

} // namespace muen::plugins::math

namespace js {

template<>
auto try_as<::Vector2>(::JSContext *js, ::JSValueConst value) -> std::expected<::Vector2, ::JSValue>;

template<>
auto try_as<::Rectangle>(::JSContext *js, ::JSValueConst value) -> std::expected<::Rectangle, ::JSValue>;

} // namespace js

template<>
struct std::formatter<Vector2> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Vector2& v, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", muen::plugins::math::vector2::to_string(v));
    }
};

template<>
struct std::formatter<Rectangle> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Rectangle& v, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", muen::plugins::math::rectangle::to_string(v));
    }
};

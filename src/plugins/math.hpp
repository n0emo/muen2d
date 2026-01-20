#pragma once

#include <raylib.h>
#include <spdlog/spdlog.h>

#include <engine/plugin.hpp>
#include <quickjs.hpp>

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

template<>
struct fmt::formatter<Vector2> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Vector2& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::math::vector2::to_string(v));
    }
};

template<>
struct fmt::formatter<Rectangle> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Rectangle& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::math::rectangle::to_string(v));
    }
};

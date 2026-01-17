#pragma once

#include <expected>

#include <raylib.h>
#include <spdlog/spdlog.h>

#include <engine/plugin.hpp>
#include <jsutil.hpp>

namespace js {

template<>
auto try_as<Camera2D>(JSContext *js, JSValueConst value) -> std::expected<Camera2D, JSValue>;

template<>
auto try_as<Color>(JSContext *js, JSValueConst value) -> std::expected<Color, JSValue>;

template<>
auto try_as<Font>(JSContext *js, JSValueConst value) -> std::expected<Font, JSValue>;

template<>
auto try_as<NPatchInfo>(JSContext *js, JSValueConst value) -> std::expected<NPatchInfo, JSValue>;

template<>
auto try_as<Texture>(JSContext *js, JSValueConst value) -> std::expected<Texture, JSValue>;

template<>
auto try_as<RenderTexture>(JSContext *js, JSValueConst value) -> std::expected<RenderTexture, JSValue>;

} // namespace js

namespace muen::plugins::graphics {

auto module(JSContext *js) -> JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

namespace camera {
    extern const JSClassDef CLASS;
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto to_string(Camera2D camera) -> std::string;
} // namespace camera

namespace color {
    extern const JSClassDef CLASS;
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto to_string(Color color) -> std::string;
} // namespace color

namespace font {
    extern const JSClassDef CLASS;
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto to_string(Font font) -> std::string;
} // namespace font

namespace npatch {
    extern const JSClassDef CLASS;
    auto module(::JSContext *js) -> ::JSModuleDef *;
    auto to_string(NPatchInfo npatch) -> std::string;
} // namespace npatch

namespace render_texture {
    extern const JSClassDef CLASS;
    auto module(JSContext *js) -> JSModuleDef *;
    auto to_string(RenderTexture texture) -> std::string;
} // namespace render_texture

namespace texture {
    extern const JSClassDef CLASS;
    auto module(JSContext *js) -> JSModuleDef *;
    auto to_string(Texture texture) -> std::string;
} // namespace texture

} // namespace muen::plugins::graphics

template<>
struct fmt::formatter<Camera2D> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Camera2D& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::graphics::camera::to_string(v));
    }
};

template<>
struct fmt::formatter<Color> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Color& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::graphics::color::to_string(v));
    }
};

template<>
struct fmt::formatter<Font> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Font& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::graphics::font::to_string(v));
    }
};

template<>
struct fmt::formatter<NPatchInfo> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const NPatchInfo& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::graphics::npatch::to_string(v));
    }
};

template<>
struct fmt::formatter<RenderTexture> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const RenderTexture& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::graphics::render_texture::to_string(v));
    }
};

template<>
struct fmt::formatter<Texture> {
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Texture& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", muen::plugins::graphics::texture::to_string(v));
    }
};

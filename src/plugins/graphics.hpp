#pragma once

#include <raylib.h>
#include <spdlog/spdlog.h>

#include <engine/plugin.hpp>
#include <quickjs.hpp>

namespace muen::js {

template<>
auto try_into<Camera2D *>(const Value& val) noexcept -> JSResult<Camera2D *>;

template<>
auto try_into<Camera2D>(const Value& val) noexcept -> JSResult<Camera2D>;

template<>
auto try_into<Color *>(const Value& val) noexcept -> JSResult<Color *>;

template<>
auto try_into<Color>(const Value& val) noexcept -> JSResult<Color>;

template<>
auto try_into<Font *>(const Value& val) noexcept -> JSResult<Font *>;

template<>
auto try_into<Font>(const Value& val) noexcept -> JSResult<Font>;

template<>
auto try_into<NPatchInfo *>(const Value& val) noexcept -> JSResult<NPatchInfo *>;

template<>
auto try_into<NPatchInfo>(const Value& val) noexcept -> JSResult<NPatchInfo>;

template<>
auto try_into<Texture *>(const Value& val) noexcept -> JSResult<Texture *>;

template<>
auto try_into<Texture>(const Value& val) noexcept -> JSResult<Texture>;

template<>
auto try_into<RenderTexture *>(const Value& val) noexcept -> JSResult<RenderTexture *>;

template<>
auto try_into<RenderTexture>(const Value& val) noexcept -> JSResult<RenderTexture>;

} // namespace muen::js

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
    extern const JSClassDef RENDER_TEXTURE;
    auto module(JSContext *js) -> JSModuleDef *;
    auto to_string(RenderTexture texture) -> std::string;
} // namespace render_texture

namespace texture {
    extern const JSClassDef TEXTURE;
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

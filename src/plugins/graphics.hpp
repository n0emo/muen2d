#pragma once

#include <variant>

#include <raylib.hpp>
#include <spdlog/spdlog.h>

#include <engine/plugin.hpp>
#include <quickjs.hpp>
#include <data.hpp>

namespace muen::js {

template<>
auto try_into<Camera2D>(const Value& val) noexcept -> JSResult<Camera2D>;

template<>
auto try_into<Color>(const Value& val) noexcept -> JSResult<Color>;

template<>
auto try_into<const rl::Font *>(const Value& val) noexcept -> JSResult<const rl::Font *>;

template<>
auto try_into<NPatchInfo>(const Value& val) noexcept -> JSResult<NPatchInfo>;

template<>
auto try_into<const rl::Texture *>(const Value& val) noexcept -> JSResult<const rl::Texture *>;

template<>
auto try_into<const rl::RenderTexture *>(const Value& val) noexcept -> JSResult<const rl::RenderTexture *>;

} // namespace muen::js

namespace muen::plugins::graphics {

using namespace js;
using namespace gsl;

auto module(JSContext *js) -> JSModuleDef *;
auto plugin(JSContext *js) -> EnginePlugin;

namespace camera {
    struct CameraClassData {
        Camera2D camera {};

        static auto from_value(const Value& val) -> JSResult<CameraClassData *>;
    };

    extern const JSClassDef CAMERA;
    auto module(::JSContext *js) -> ::JSModuleDef *;
} // namespace camera

namespace color {
    struct ColorClassData {
        Color color {};

        static auto from_value(const Value& val) -> JSResult<ColorClassData *>;
    };

    extern const JSClassDef COLOR;
    auto module(::JSContext *js) -> ::JSModuleDef *;
} // namespace color

namespace font {
    struct FontClassData {
        ResourceStore<FontData>::Handle handle;
    };

    struct FontOptions {
        std::optional<std::filesystem::path> path {};
        std::optional<std::string> name {};
        std::optional<int> font_size {};
        std::optional<std::vector<int>> codepoints {};
    };

    struct FontLoadByName {
        std::string name;
    };

    struct FontLoadByParams {
        std::filesystem::path path;
        std::string name;
        int font_size;
        std::optional<std::vector<int>> codepoints;
    };

    using FontLoadMode = std::variant<std::monostate, FontLoadByName, FontLoadByParams>;

    extern const JSClassDef CLASS;
    auto module(::JSContext *js) -> ::JSModuleDef *;
} // namespace font

namespace npatch {
    struct NPatchClassData {
        NPatchInfo npatch {};

        static auto from_value(const Value& val) -> JSResult<NPatchClassData *>;
    };

    extern const JSClassDef NPATCH;
    auto module(::JSContext *js) -> ::JSModuleDef *;
} // namespace npatch

namespace render_texture {
    struct RenderTextureClassData {
        rl::RenderTexture texture {};

        static auto from_value(const Value& val) -> JSResult<RenderTextureClassData *>;
    };

    extern const JSClassDef RENDER_TEXTURE;
    auto module(JSContext *js) -> JSModuleDef *;
    auto to_string(RenderTexture texture) -> std::string;
} // namespace render_texture

namespace texture {
    struct TextureClassData {
        ResourceStore<TextureData>::Handle handle;
    };

    struct TextureOptions {
        std::optional<std::filesystem::path> path {};
        std::optional<std::string> name {};
        // TODO: image
    };

    struct TextureLoadByName {
        std::string name;
    };

    struct TextureLoadByParams {
        std::filesystem::path path;
        std::string name;
    };

    using TextureLoadMode = std::variant<TextureLoadByName, TextureLoadByParams>;

    extern const JSClassDef TEXTURE;
    auto module(JSContext *js) -> JSModuleDef *;
} // namespace texture

} // namespace muen::plugins::graphics

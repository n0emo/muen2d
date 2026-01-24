#pragma once

#include <gsl/gsl>
#include <span>
#include <string>

#include <fmt/format.h>

#include <raylib.h>

namespace rl {

using gsl::czstring;

constexpr auto MUSIC_AUDIO_NONE = 0;
constexpr auto MUSIC_AUDIO_WAV = 1;
constexpr auto MUSIC_AUDIO_OGG = 2;
constexpr auto MUSIC_AUDIO_FLAC = 3;
constexpr auto MUSIC_AUDIO_MP3 = 4;
constexpr auto MUSIC_AUDIO_QOA = 5;
constexpr auto MUSIC_MODULE_XM = 6;
constexpr auto MUSIC_MODULE_MOD = 7;

constexpr inline auto display_pixel_format(int t) noexcept -> czstring {
    switch (t) {
        case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
            return "grayscale, 8 bpp (no alpha)";
        case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
            return "gray alpha, 8*2 bpp (2 channels)";
        case PIXELFORMAT_UNCOMPRESSED_R5G6B5:
            return "R5G6B5, 16 bpp";
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
            return "R8G8B8, 24 bpp";
        case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
            return "R5G5B5A1, 16 bpp (1 bit alpha)";
        case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
            return "R4G4B4A4, 16 bpp (4 bit alpha)";
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
            return "R8G8B8A8, 32 bpp";
        case PIXELFORMAT_UNCOMPRESSED_R32:
            return "R32, 32 bpp (1 channel - float)";
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32:
            return "R32G32B32, 32*3 bpp (3 channels - float)";
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
            return "R32G32B32A32, 32*4 bpp (4 channels - float)";
        case PIXELFORMAT_UNCOMPRESSED_R16:
            return "R16, 16 bpp (1 channel - half float)";
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16:
            return "R16G16B16, 16*3 bpp (3 channels - half float)";
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
            return "R16G16B16A16, 16*4 bpp (4 channels - half float)";
        case PIXELFORMAT_COMPRESSED_DXT1_RGB:
            return "DXT1 RGB, 4 bpp (no alpha)";
        case PIXELFORMAT_COMPRESSED_DXT1_RGBA:
            return "DXT1 RGBA, 4 bpp (1 bit alpha)";
        case PIXELFORMAT_COMPRESSED_DXT3_RGBA:
            return "DXT3 RGBA, 8 bpp";
        case PIXELFORMAT_COMPRESSED_DXT5_RGBA:
            return "DXT5 RGBA, 8 bpp";
        case PIXELFORMAT_COMPRESSED_ETC1_RGB:
            return "ETC1 RGB, 4 bpp";
        case PIXELFORMAT_COMPRESSED_ETC2_RGB:
            return "ETC2 RGB, 4 bpp";
        case PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA:
            return "ETC2 EAC RGBA, 8 bpp";
        case PIXELFORMAT_COMPRESSED_PVRT_RGB:
            return "PVRT RGB, 4 bpp";
        case PIXELFORMAT_COMPRESSED_PVRT_RGBA:
            return "PVRT RGBA, 4 bpp";
        case PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA:
            return "ASTC 4x4 RGBA, 8 bpp";
        case PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA:
            return "ASTC 8x8 RGBA, 2 bpp";
        default:
            return "unknown";
    }
}

constexpr inline auto display_npatch_layout(int t) noexcept -> czstring {
    switch (t) {
        case NPATCH_NINE_PATCH:
            return "default";
        case NPATCH_THREE_PATCH_VERTICAL:
            return "vertical";
        case NPATCH_THREE_PATCH_HORIZONTAL:
            return "horizontal";
        default:
            return "unknown";
    }
}

constexpr inline auto display_projection(int t) noexcept -> czstring {
    switch (t) {
        case CAMERA_PERSPECTIVE:
            return "perspective";
        case CAMERA_ORTHOGRAPHIC:
            return "orthographics";
        default:
            return "unknown";
    }
}

constexpr inline auto display_music_type(int t) noexcept -> czstring {
    switch (t) {
        case rl::MUSIC_AUDIO_NONE:
            return "none";
        case rl::MUSIC_AUDIO_WAV:
            return "wav";
        case rl::MUSIC_AUDIO_OGG:
            return "ogg";
        case rl::MUSIC_AUDIO_FLAC:
            return "flac";
        case rl::MUSIC_AUDIO_MP3:
            return "mp3";
        case rl::MUSIC_AUDIO_QOA:
            return "qoa";
        case rl::MUSIC_MODULE_XM:
            return "xm";
        case rl::MUSIC_MODULE_MOD:
            return "mod";
        default:
            return "unknown";
    }
}

class Image: public ::Image {
  public:
    static auto load(czstring file_name) noexcept -> Image {
        return {::LoadImage(file_name)};
    }

    static auto load_raw(czstring file_name, int width, int height, int format, int header_size) noexcept -> Image {
        return {::LoadImageRaw(file_name, width, height, format, header_size)};
    }

    static auto load_anim(czstring file_name, int *frames) noexcept -> Image {
        return {::LoadImageAnim(file_name, frames)};
    }

    static auto load_anim_from_memory(czstring file_type, std::span<unsigned char> data, int *frames) -> Image {
        return {::LoadImageAnimFromMemory(file_type, data.data(), int(data.size()), frames)};
    }

    static auto load_from_memory(czstring file_type, std::span<unsigned char> data) -> Image {
        return {::LoadImageFromMemory(file_type, data.data(), int(data.size()))};
    }

    static auto load_from_texture(::Texture texture) -> Image {
        return {::LoadImageFromTexture(texture)};
    }

    static auto load_from_screen() -> Image {
        return {::LoadImageFromScreen()};
    }

    Image() noexcept : ::Image {} {}

    Image(const Image&) = delete;

    Image(Image&& other) noexcept : ::Image {other} {
        other.reset();
    };

    auto operator=(const Image&) -> Image& = delete;

    auto operator=(Image&& other) noexcept -> Image& {
        swap(*this, other);
        return *this;
    }

    ~Image() noexcept {
        ::UnloadImage(*this);
    }

    friend inline auto swap(Image& a, Image& b) noexcept -> void;

  private:
    Image(::Image img) noexcept : ::Image {img} {}

    auto reset() noexcept -> void {
        data = {};
        width = {};
        height = {};
        mipmaps = {};
        format = {};
    }
};

class Texture: public ::Texture {
  public:
    static auto load(czstring file_name) noexcept -> Texture {
        return {::LoadTexture(file_name)};
    }

    static auto load_from_image(::Image image) noexcept -> Texture {
        return {::LoadTextureFromImage(image)};
    }

    static auto load_from_memory(czstring extension, std::span<unsigned char> data) noexcept -> Texture {
        const auto image = ::LoadImageFromMemory(extension, data.data(), int(data.size()));
        if (!::IsImageValid(image)) return {};
        const auto texture = ::LoadTextureFromImage(image);
        ::UnloadImage(image);
        return {texture};
    }

    Texture() noexcept : ::Texture {} {}

    Texture(Texture&& other) noexcept : ::Texture {other} {
        other.reset();
    }

    auto operator=(Texture&& other) noexcept -> Texture& {
        swap(*this, other);
        return *this;
    }

    Texture(const Texture&) = delete;

    auto operator=(const Texture&) -> Texture& = delete;

    ~Texture() noexcept {
        ::UnloadTexture(*this);
    }

    friend inline auto swap(Texture& a, Texture& b) noexcept -> void;

  private:
    Texture(::Texture texture) noexcept : ::Texture {texture} {}

    auto reset() noexcept -> void {
        this->id = 0;
        this->width = 0;
        this->height = 0;
        this->mipmaps = 0;
        this->format = 0;
    }
};

class RenderTexture: public ::RenderTexture {
  public:
    static auto load(int width, int height) noexcept -> RenderTexture {
        return {::LoadRenderTexture(width, height)};
    }

    RenderTexture() noexcept : ::RenderTexture {} {};

    RenderTexture(const RenderTexture&) = delete;

    RenderTexture(RenderTexture&& other) noexcept : ::RenderTexture {other} {
        other.reset();
    };

    auto operator=(const RenderTexture&) -> RenderTexture& = delete;

    auto operator=(RenderTexture&& other) noexcept -> RenderTexture& {
        swap(*this, other);
        return *this;
    };

    ~RenderTexture() noexcept {
        ::UnloadRenderTexture(*this);
    }

    friend inline auto swap(RenderTexture& a, RenderTexture& b) noexcept -> void;

  private:
    RenderTexture(::RenderTexture texture) noexcept : ::RenderTexture {texture} {}

    auto reset() noexcept -> void {
        id = {};
        texture = {};
        depth = {};
    }
};

class Font: public ::Font {
  public:
    static auto load(czstring file_name) noexcept -> Font {
        return {::LoadFont(file_name)};
    }

    static auto load_ex(czstring file_name, int font_size, std::optional<std::span<int>> codepoints) noexcept -> Font {
        if (codepoints) return {::LoadFontEx(file_name, font_size, codepoints->data(), int(codepoints->size()))};
        else return {::LoadFontEx(file_name, font_size, nullptr, 0)};
    }

    static auto load_from_image(::Image image, ::Color key, int first_char) noexcept -> Font {
        return {::LoadFontFromImage(image, key, first_char)};
    }

    static auto load_from_memory(
        czstring file_type,
        std::span<unsigned char> data,
        int font_size,
        std::optional<std::span<int>> codepoints
    ) noexcept -> Font {
        if (codepoints) {
            return {::LoadFontFromMemory(
                file_type,
                data.data(),
                int(data.size()),
                font_size,
                codepoints->data(),
                int(codepoints->size())
            )};
        } else {
            return {::LoadFontFromMemory(file_type, data.data(), int(data.size()), font_size, nullptr, 0)};
        }
    }

    Font() noexcept : ::Font {} {}

    Font(const Font&) = delete;

    Font(Font&& other) noexcept : ::Font {other} {
        other.reset();
    }

    auto operator=(const Font&) -> Font& = delete;

    auto operator=(Font&& other) noexcept -> Font& {
        swap(*this, other);
        return *this;
    }

    ~Font() noexcept {
        ::UnloadFont(*this);
    }

    friend inline auto swap(Font& a, Font& b) noexcept -> void;

  private:
    Font(::Font font) noexcept : ::Font {font} {}

    auto reset() noexcept -> void {
        baseSize = {};
        glyphCount = {};
        glyphPadding = {};
        texture = {};
        recs = {};
        glyphs = {};
    }
};

class Wave: public ::Wave {
  public:
    static auto load(czstring file_name) noexcept -> Wave {
        return {::LoadWave(file_name)};
    }

    static auto load_from_memory(czstring file_type, std::span<unsigned char> data) noexcept -> Wave {
        return {::LoadWaveFromMemory(file_type, data.data(), int(data.size()))};
    }

    Wave() noexcept : ::Wave {} {}

    Wave(const Wave&) = delete;

    Wave(Wave&& other) noexcept : ::Wave {other} {
        other.reset();
    }

    auto operator=(const Wave&) -> Wave& = delete;

    auto operator=(Wave&& other) noexcept -> Wave& {
        swap(*this, other);
        return *this;
    };

    ~Wave() noexcept {
        UnloadWave(*this);
    }

    friend inline auto swap(Wave& a, Wave& b) noexcept -> void;

  private:
    Wave(::Wave wave) noexcept : ::Wave {wave} {};

    auto reset() noexcept -> void {
        frameCount = {};
        sampleRate = {};
        sampleSize = {};
        channels = {};
        data = {};
    }
};

class Sound: public ::Sound {
  public:
    static auto load(czstring file_name) noexcept -> Sound {
        return {::LoadSound(file_name)};
    }

    static auto load_from_wave(::Wave wave) noexcept -> Sound {
        return {::LoadSoundFromWave(wave)};
    }

    Sound() noexcept : ::Sound {} {}

    Sound(const Sound&) = delete;

    Sound(Sound&& other) noexcept : ::Sound {other} {
        other.reset();
    }

    auto operator=(const Sound&) -> Sound& = delete;

    auto operator=(Sound&& other) noexcept -> Sound& {
        swap(*this, other);
        return *this;
    }

    ~Sound() noexcept {
        ::UnloadSound(*this);
    }

    friend inline auto swap(Sound& a, Sound& b) noexcept -> void;

  private:
    Sound(::Sound sound) noexcept : ::Sound {sound} {}

    auto reset() noexcept -> void {
        stream = {};
        frameCount = {};
    }
};

class Music: public ::Music {
  public:
    static auto load(czstring file_name) noexcept -> Music {
        return {::LoadMusicStream(file_name)};
    }

    static auto load_from_memory(czstring file_type, std::span<unsigned char> data) noexcept -> Music {
        return {::LoadMusicStreamFromMemory(file_type, data.data(), int(data.size()))};
    }

    Music() noexcept : ::Music {} {}

    Music(const Music&) = delete;

    Music(Music&& other) noexcept : ::Music {other} {
        other.reset();
    }

    auto operator=(const Music&) -> Music& = delete;

    auto operator=(Music&& other) noexcept -> Music& {
        swap(*this, other);
        return *this;
    };

    ~Music() noexcept {
        UnloadMusicStream(*this);
    }

    friend inline auto swap(Music& a, Music& b) noexcept -> void;

  private:
    Music(::Music music) noexcept : ::Music {music} {}

    auto reset() noexcept -> void {
        stream = {};
        frameCount = {};
        looping = {};
        ctxType = {};
        ctxData = {};
    }
};

inline auto swap(Image& x, Image& y) noexcept -> void {
    using std::swap;

    swap(x.data, y.data);
    swap(x.width, y.width);
    swap(x.height, y.height);
    swap(x.mipmaps, y.mipmaps);
    swap(x.format, y.format);
}

inline auto swap(Texture& x, Texture& y) noexcept -> void {
    using std::swap;

    swap(x.id, y.id);
    swap(x.width, y.width);
    swap(x.height, y.height);
    swap(x.mipmaps, y.mipmaps);
    swap(x.format, y.format);
}

inline auto swap(RenderTexture& x, RenderTexture& y) noexcept -> void {
    using std::swap;

    swap(x.id, y.id);
    swap(x.texture, y.texture);
    swap(x.depth, y.depth);
}

inline auto swap(Font& x, Font& y) noexcept -> void {
    using std::swap;

    swap(x.baseSize, y.baseSize);
    swap(x.glyphCount, y.glyphCount);
    swap(x.glyphPadding, y.glyphPadding);
    swap(x.texture, y.texture);
    swap(x.recs, y.recs);
    swap(x.glyphs, y.glyphs);
}

inline auto swap(Wave& x, Wave& y) noexcept -> void {
    using std::swap;

    swap(x.frameCount, y.frameCount);
    swap(x.sampleRate, y.sampleRate);
    swap(x.sampleSize, y.sampleSize);
    swap(x.channels, y.channels);
    swap(x.data, y.data);
}

inline auto swap(Sound& x, Sound& y) noexcept -> void {
    using std::swap;

    swap(x.stream, y.stream);
    swap(x.frameCount, y.frameCount);
}

inline auto swap(Music& x, Music& y) noexcept -> void {
    using std::swap;

    swap(x.stream, y.stream);
    swap(x.frameCount, y.frameCount);
    swap(x.looping, y.looping);
    swap(x.ctxType, y.ctxType);
    swap(x.ctxData, y.ctxData);
}

} // namespace rl

template<>
struct fmt::formatter<::Vector2>: public fmt::formatter<std::string> {
    auto format(const ::Vector2& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};

template<>
struct fmt::formatter<::Vector3>: public fmt::formatter<std::string> {
    auto format(const ::Vector3& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};

template<>
struct fmt::formatter<::Vector4>: public fmt::formatter<std::string> {
    auto format(const ::Vector4& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};

template<>
struct fmt::formatter<::Matrix>: public fmt::formatter<std::string> {
    auto format(const ::Matrix& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Matrix {{ {} {} {} {} ; {} {} {} {} ; {} {} {} {} ; {} {} {} {} }}",
            v.m0,
            v.m4,
            v.m8,
            v.m12,
            v.m1,
            v.m5,
            v.m9,
            v.m13,
            v.m2,
            v.m6,
            v.m10,
            v.m14,
            v.m3,
            v.m7,
            v.m11,
            v.m15
        );
    }
};

template<>
struct fmt::formatter<::Color>: public fmt::formatter<std::string> {
    auto format(const ::Color& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "Color {{ r: {}, g: {}, b: {}, a: {} }}", v.r, v.g, v.b, v.a);
    }
};

template<>
struct fmt::formatter<::Rectangle>: public fmt::formatter<std::string> {
    auto format(const ::Rectangle& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Rectangle {{ position: ({}, {}), size: ({}, {}) }}",
            v.x,
            v.y,
            v.width,
            v.height
        );
    }
};

template<>
struct fmt::formatter<::Image>: public fmt::formatter<std::string> {
    auto format(const ::Image& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Image {{ data: {}, width: {}, height: {}, mipmaps: {}, format: {} }}",
            v.data,
            v.width,
            v.height,
            v.mipmaps,
            rl::display_pixel_format(v.format)
        );
    }
};

template<>
struct fmt::formatter<rl::Image>: public fmt::formatter<::Image> {};

template<>
struct fmt::formatter<::Texture>: public fmt::formatter<std::string> {
    auto format(const Texture& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Texture {{ id: {}, size: {}x{}, mipmaps: {}, format: {} }}",
            v.id,
            v.width,
            v.height,
            v.mipmaps,
            rl::display_pixel_format(v.format)
        );
    }
};

template<>
struct fmt::formatter<rl::Texture>: public fmt::formatter<::Texture> {};

template<>
struct fmt::formatter<::RenderTexture>: public fmt::formatter<std::string> {
    auto format(const RenderTexture& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "RenderTexture {{ id: {}, texture: {}, depth: {} }}",
            v.id,
            v.texture,
            v.depth
        );
    }
};

template<>
struct fmt::formatter<rl::RenderTexture>: public fmt::formatter<::RenderTexture> {};

template<>
struct fmt::formatter<::NPatchInfo>: public fmt::formatter<std::string> {
    auto format(const ::NPatchInfo& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "NPatchInfo {{ source: {}, left: {}, top: {}, right: {}, bottom: {}, layout: {} }}",
            v.source,
            v.left,
            v.top,
            v.right,
            v.bottom,
            rl::display_npatch_layout(v.layout)
        );
    }
};

template<>
struct fmt::formatter<::GlyphInfo>: public fmt::formatter<std::string> {
    auto format(const ::GlyphInfo& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "GlyphInfo {{ value: {}, offset: ({}, {}), advanceX: {}, image: {} }}",
            v.value,
            v.offsetX,
            v.offsetY,
            v.advanceX,
            v.image
        );
    }
};

template<>
struct fmt::formatter<::Font>: public fmt::formatter<std::string> {
    auto format(const ::Font& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Font {{ texture: {}, baseSize: {}, glyphCount: {}, glyphPadding: {} }}",
            v.texture,
            v.baseSize,
            v.glyphCount,
            v.glyphPadding
        );
    }
};

template<>
struct fmt::formatter<rl::Font>: public fmt::formatter<::Font> {};

template<>
struct fmt::formatter<::Camera3D>: public fmt::formatter<std::string> {
    auto format(const ::Camera3D& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Camera3D {{ position: {}, target: {}, up: {}, fovy: {}, projection: {} }}",
            v.position,
            v.target,
            v.up,
            v.fovy,
            rl::display_projection(v.projection)
        );
    }
};

template<>
struct fmt::formatter<::Camera2D>: public fmt::formatter<std::string> {
    auto format(const ::Camera2D& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Camera2D {{ offset: {}, target: {}, rotation: {}, zoom: {} }}",
            v.offset,
            v.target,
            v.rotation,
            v.zoom
        );
    }
};

template<>
struct fmt::formatter<::AudioStream>: public fmt::formatter<std::string> {
    auto format(const ::AudioStream& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "AudioStream {{ sampleRate: {}, sampleSize: {}, channels: {} }}",
            v.sampleRate,
            v.sampleSize,
            v.channels
        );
    }
};

template<>
struct fmt::formatter<::Sound>: public fmt::formatter<std::string> {
    auto format(const ::Sound& v, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "Sound {{ frameCount: {}, stream: {} }}", v.frameCount, v.stream);
    }
};

template<>
struct fmt::formatter<rl::Sound>: public fmt::formatter<::Sound> {};

template<>
struct fmt::formatter<::Music>: public fmt::formatter<std::string> {
    auto format(const ::Music& v, fmt::format_context& ctx) const {
        return fmt::format_to(
            ctx.out(),
            "Music {{ frameCount: {}, looping: {}, ctxType: {}, stream: {} }}",
            v.frameCount,
            v.looping,
            rl::display_music_type(v.ctxType),
            v.stream
        );
    }
};

template<>
struct fmt::formatter<rl::Music>: public fmt::formatter<::Music> {};

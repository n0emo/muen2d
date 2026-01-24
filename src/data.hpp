#pragma once

#include <string>
#include <span>
#include <filesystem>

#include <raylib.hpp>
#include <resource_store.hpp>

namespace muen {

struct TextureData {
    rl::Texture texture;
    std::filesystem::path name;

    using data_type = rl::Texture;

    auto get() noexcept -> rl::Texture& {
        return texture;
    }

    static auto load(const std::filesystem::path& name, IFileStore& file_store) noexcept -> TextureData try {
        auto buf = file_store.read_bytes(name);
        if (!buf) {
            SPDLOG_WARN("Could not load texture {}: {}", name.string(), buf.error()->msg());
            return {};
        }

        return load_from_memory(name, *buf);
    } catch (...) {
        return {};
    }

    static auto load_from_memory(const std::filesystem::path& name, std::span<char> buf) noexcept -> TextureData try {
        // NOLINTNEXTLINE: cast from unsigned char* to char* is safe
        const auto data = std::span(reinterpret_cast<unsigned char *>(buf.data()), int(buf.size()));
        auto texture = rl::Texture::load_from_memory(name.extension().string().c_str(), data);
        return {.texture = std::move(texture), .name = name};
    } catch (...) {
        return {};
    }
};

struct FontData {
    rl::Font font;
    std::filesystem::path name;

    using data_type = rl::Font;

    auto get() noexcept -> rl::Font& {
        return font;
    }

    static auto load(
        const std::filesystem::path& name,
        int font_size,
        std::optional<std::span<int>> codepoints,
        IFileStore& file_store
    ) noexcept -> FontData try {
        auto buf = file_store.read_bytes(name);
        if (!buf) {
            SPDLOG_WARN("Could not load font {}: {}", name.string(), buf.error()->msg());
            return {};
        }

        return load_from_memory(name, *buf, font_size, codepoints);
    } catch (...) {
        return {};
    }

    static auto load_from_memory(
        const std::filesystem::path& name,
        std::span<char> buf,
        int font_size,
        std::optional<std::span<int>> codepoints
    ) noexcept -> FontData try {
        // NOLINTNEXTLINE: cast from unsigned char* to char* is safe
        auto data = std::span(reinterpret_cast<unsigned char *>(buf.data()), int(buf.size()));
        auto font = rl::Font::load_from_memory(name.extension().string().c_str(), data, font_size, codepoints);
        return {.font = std::move(font), .name = name};
    } catch (...) {
        return {};
    }
};

} // namespace muen

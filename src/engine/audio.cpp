#include "./audio.hpp"

#include <raylib.h>

namespace glint::engine::audio {

auto init() noexcept -> void {
    InitAudioDevice();
}

auto close() noexcept -> void {
    // TODO: free sounds
    CloseAudioDevice();
}

auto get() noexcept -> Audio& {
    static auto audio = Audio {};
    return audio;
}

} // namespace glint::plugins::audio

#include "./audio.hpp"

#include <raylib.h>

namespace muen::engine::audio {

auto init() -> void {
    ::InitAudioDevice();
}

auto close() -> void {
    // TODO: free sounds
    ::CloseAudioDevice();
}

auto get() -> Audio& {
    static auto audio = Audio {};
    return audio;
}

} // namespace muen::plugins::audio

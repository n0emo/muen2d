#include "./audio.hpp"

#include <algorithm>

namespace muen::engine::audio::sound {

auto load(const std::filesystem::path& name, std::span<char> data) noexcept -> Result<owner<Sound *>> {
    const auto raylib_wave = LoadWaveFromMemory(
        name.extension().string().c_str(),
        // NOLINTNEXTLINE: Casting from char* to unsigned char* is explicitly allowed by the standard
        reinterpret_cast<unsigned char *>(data.data()),
        int(data.size())
    );

    const auto raylib_sound = LoadSoundFromWave(raylib_wave);
    UnloadWave(raylib_wave);

    if (!::IsSoundValid(raylib_sound)) {
        return err("Error loading sound");
    }

    const auto sound = Sound {.sound = raylib_sound};
    ::SetSoundVolume(sound.sound, sound.volume);
    ::SetSoundPan(sound.sound, sound.pan);
    ::SetSoundPitch(sound.sound, sound.pitch);

    return {owner<Sound *> {new (std::nothrow) Sound {sound}}};
}

auto unload(owner<Sound *> self) noexcept -> void {
    if (self == nullptr) {
        return;
    }

    UnloadSound(self->sound);
    delete self;
}

auto play(Sound& self) noexcept -> void {
    PlaySound(self.sound);
}

auto stop(Sound& self) noexcept -> void {
    StopSound(self.sound);
}

auto pause(Sound& self) noexcept -> void {
    PauseSound(self.sound);
}

auto resume(Sound& self) noexcept -> void {
    ResumeSound(self.sound);
}

auto is_playing(Sound& self) noexcept -> bool {
    return IsSoundPlaying(self.sound);
}

auto get_volume(Sound& self) noexcept -> float {
    return self.volume;
}

auto set_volume(Sound& self, float volume) noexcept -> void {
    self.volume = std::clamp(volume, 0.0f, 1.0f);
    SetSoundVolume(self.sound, self.volume);
}

auto get_pan(Sound& self) noexcept -> float {
    return self.pan;
}

auto set_pan(Sound& self, float pan) noexcept -> void {
    self.pan = std::clamp(pan, 0.0f, 1.0f);
    SetSoundPan(self.sound, self.pan);
}

auto get_pitch(Sound& self) noexcept -> float {
    return self.pitch;
}

auto set_pitch(Sound& self, float pitch) noexcept -> void {
    self.pitch = pitch;
    SetSoundPitch(self.sound, pitch);
}

} // namespace muen::engine::audio::sound

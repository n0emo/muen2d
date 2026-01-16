#include "./audio.hpp"

#include <algorithm>

namespace muen::engine::audio::sound {

auto load(const std::string& path) -> Result<Sound*> {
    const auto raylib_sound = ::LoadSound(path.c_str());
    if (!::IsSoundValid(raylib_sound)) {
        return Err(error::create("Error loading sound"));
    }

    const auto sound = Sound {.sound = raylib_sound};
    ::SetSoundVolume(sound.sound, sound.volume);
    ::SetSoundPan(sound.sound, sound.pan);
    ::SetSoundPitch(sound.sound, sound.pitch);

    return new Sound{sound};
}

auto unload(Sound* self) -> void {
    if (self == nullptr) {
        return;
    }

    ::UnloadSound(self->sound);
    delete self;
}

auto play(Sound& self) -> void {
    ::PlaySound(self.sound);
}

auto stop(Sound& self) -> void {
    ::StopSound(self.sound);
}

auto pause(Sound& self) -> void {
    ::PauseSound(self.sound);
}

auto resume(Sound& self) -> void {
    ::ResumeSound(self.sound);
}

auto is_playing(Sound& self) -> bool {
    return ::IsSoundPlaying(self.sound);
}

auto get_volume(Sound& self) -> float {
    return self.volume;
}

auto set_volume(Sound& self, float volume) -> void {
    self.volume = std::clamp(volume, 0.0f, 1.0f);
    ::SetSoundVolume(self.sound, self.volume);
}

auto get_pan(Sound& self) -> float {
    return self.pan;
}

auto set_pan(Sound& self, float pan) -> void {
    self.pan = std::clamp(pan, 0.0f, 1.0f);
    ::SetSoundPan(self.sound, self.pan);
}

auto get_pitch(Sound& self) -> float {
    return self.pitch;
}

auto set_pitch(Sound& self, float pitch) -> void {
    self.pitch = pitch;
    ::SetSoundPitch(self.sound, pitch);
}

} // namespace muen::plugins::audio::sound

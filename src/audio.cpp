#include "audio.hpp"

#include <algorithm>

namespace muen::audio {

auto create() -> Audio {
    InitAudioDevice();
    return Audio {};
}

auto close(Audio& self) -> void {
    (void)self;
    CloseAudioDevice();
}

namespace sound {

    auto load(const std::string& path) -> std::expected<Sound, std::string> {
        const auto raylib_sound = ::LoadSound(path.c_str());
        if (!::IsSoundValid(raylib_sound)) {
            return std::unexpected("Error loading sound");
        }

        const auto sound = Sound {.sound = raylib_sound};
        ::SetSoundVolume(sound.sound, sound.volume);
        ::SetSoundPan(sound.sound, sound.pan);
        ::SetSoundPitch(sound.sound, sound.pitch);

        return sound;
    }

    auto unload(Sound& self) -> void {
        ::UnloadSound(self.sound);
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

} // namespace sound

namespace music {

    auto load(const std::string& path) -> std::expected<Music, std::string> {
        const auto raylib_music = ::LoadMusicStream(path.c_str());
        if (!::IsMusicValid(raylib_music)) {
            return std::unexpected("Error loading music");
        }

        const auto music = Music {.music = raylib_music};
        ::SetMusicVolume(music.music, music.volume);
        ::SetMusicPan(music.music, music.pan);
        ::SetMusicPitch(music.music, music.pitch);

        return music;
    }

    auto unload(Music& self) -> void {
        ::UnloadMusicStream(self.music);
    }

    auto update(Music& self) -> void {
        ::UpdateMusicStream(self.music);
    }

    auto play(Music& self) -> void {
        ::PlayMusicStream(self.music);
    }

    auto stop(Music& self) -> void {
        ::StopMusicStream(self.music);
    }

    auto pause(Music& self) -> void {
        ::PauseMusicStream(self.music);
    }

    auto resume(Music& self) -> void {
        ::ResumeMusicStream(self.music);
    }

    auto seek(Music& self, float cursor) -> void {
        ::SeekMusicStream(self.music, cursor);
    }

    auto is_playing(const Music& self) -> bool {
        return ::IsMusicStreamPlaying(self.music);
    }

    auto get_looping(const Music& self) -> bool {
        return self.music.looping;
    }

    auto set_looping(Music& self, bool looping) -> void {
        self.music.looping = looping;
    }

    auto get_volume(const Music& self) -> float {
        return self.volume;
    }

    auto set_volume(Music& self, float volume) -> void {
        self.volume = std::clamp(volume, 0.0f, 1.0f);
        ::SetMusicVolume(self.music, self.volume);
    }

    auto get_pan(const Music& self) -> float {
        return self.pan;
    }

    auto set_pan(Music& self, float pan) -> void {
        self.pan = std::clamp(pan, 0.0f, 1.0f);
        ::SetMusicPan(self.music, self.pan);
    }

    auto get_pitch(const Music& self) -> float {
        return self.pitch;
    }

    auto set_pitch(Music& self, float pitch) -> void {
        self.pitch = pitch;
        ::SetMusicPitch(self.music, self.pitch);
    }

} // namespace music

} // namespace muen::audio

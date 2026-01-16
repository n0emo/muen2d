#include "./audio.hpp"

#include <cstring>
#include <algorithm>

namespace muen::engine::audio::music {

auto load(const std::string& path) -> Result<Music *> {
    const auto raylib_music = ::LoadMusicStream(path.c_str());
    if (!::IsMusicValid(raylib_music)) {
        return Err(error::create("Error loading music"));
    }

    const auto music = Music {.music = raylib_music};
    ::SetMusicVolume(music.music, music.volume);
    ::SetMusicPan(music.music, music.pan);
    ::SetMusicPitch(music.music, music.pitch);

    return new Music {music};
}

auto unload(Music *self) -> void {
    ::UnloadMusicStream(self->music);
    delete self;
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

} // namespace muen::plugins::audio::music

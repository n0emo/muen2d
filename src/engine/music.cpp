#include "./audio.hpp"

#include <algorithm>
#include <gsl/gsl>

#include <spdlog/spdlog.h>

namespace muen::engine::audio::music {

using namespace gsl;

auto load(const std::filesystem::path& name, IFileStore &store) noexcept -> Result<Music> {
    auto data = store.read_bytes(name);
    if (!data) return err(data);
    auto buf = std::make_unique<unsigned char[]>(data->size());
    std::ranges::copy(*data, buf.get());
    auto span = std::span{buf.get(), data->size()};
    auto raylib_music = rl::Music::load_from_memory(name.extension().string().c_str(), span);

    auto music = Music {.music = std::move(raylib_music), .data = std::move(buf)};

    SetMusicVolume(music.music, music.volume);
    SetMusicPan(music.music, music.pan);
    SetMusicPitch(music.music, music.pitch);

    return music;
}

auto update(Music& self) noexcept -> void {
    UpdateMusicStream(self.music);
}

auto play(Music& self) noexcept -> void {
    PlayMusicStream(self.music);
}

auto stop(Music& self) noexcept -> void {
    StopMusicStream(self.music);
}

auto pause(Music& self) noexcept -> void {
    PauseMusicStream(self.music);
}

auto resume(Music& self) noexcept -> void {
    ResumeMusicStream(self.music);
}

auto seek(Music& self, float cursor) noexcept -> void {
    SeekMusicStream(self.music, cursor);
}

auto is_playing(const Music& self) noexcept -> bool {
    return IsMusicStreamPlaying(self.music);
}

auto get_looping(const Music& self) noexcept -> bool {
    return self.music.looping;
}

auto set_looping(Music& self, bool looping) noexcept -> void {
    self.music.looping = looping;
}

auto get_volume(const Music& self) noexcept -> float {
    return self.volume;
}

auto set_volume(Music& self, float volume) noexcept -> void {
    self.volume = std::clamp(volume, 0.0f, 1.0f);
    SetMusicVolume(self.music, self.volume);
}

auto get_pan(const Music& self) noexcept -> float {
    return self.pan;
}

auto set_pan(Music& self, float pan) noexcept -> void {
    self.pan = std::clamp(pan, 0.0f, 1.0f);
    SetMusicPan(self.music, self.pan);
}

auto get_pitch(const Music& self) noexcept -> float {
    return self.pitch;
}

auto set_pitch(Music& self, float pitch) noexcept -> void {
    self.pitch = pitch;
    ::SetMusicPitch(self.music, self.pitch);
}

} // namespace muen::engine::audio::music

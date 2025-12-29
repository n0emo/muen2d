#include "music.hpp"

#include "audio.hpp"
#include "engine.hpp"
#include "bindings.hpp"

namespace muen::bindings::music {

namespace music = audio::music;

auto define(js::State *j) -> void {
    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto filename = js::tostring(j, 1);
            const auto result = engine::load_music(*e, filename);
            if (!result.has_value()) {
                js::error(j, "Could not load music: %s", result.error().c_str());
            }
            js::pushnumber(j, double(result.value()));
        },
        "__muenMusicLoad",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            engine::unload_music(*e, id);
            js::pushundefined(j);
        },
        "__muenMusicUnload",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not play invalid music");
            }
            music::play(music.value());
            js::pushundefined(j);
        },
        "__muenMusicPlay",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not stop invalid music");
            }
            music::stop(music.value());
            js::pushundefined(j);
        },
        "__muenMusicStop",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not pause invalid music");
            }
            music::pause(music.value());
            js::pushundefined(j);
        },
        "__muenMusicPause",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not resume invalid music");
            }
            music::resume(music.value());
            js::pushundefined(j);
        },
        "__muenMusicResume",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            const auto cursor = js::tofloat(j, 2);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not seek invalid music");
            }
            music::seek(music.value(), cursor);
            js::pushundefined(j);
        },
        "__muenMusicSeek",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            const auto result = music::is_playing(music.value());
            js::pushboolean(j, result);
        },
        "__muenMusicIsPlaying",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            const auto result = music::get_looping(music.value());
            js::pushboolean(j, result);
        },
        "__muenMusicGetLooping",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            const auto looping = js::toboolean(j, 2);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            music::set_looping(music.value(), looping);
            js::pushundefined(j);
        },
        "__muenMusicSetLooping",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            const auto result = music::get_volume(music.value());
            js::pushnumber(j, result);
        },
        "__muenMusicGetVolume",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            const auto volume = js::tofloat(j, 2);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            music::set_volume(music.value(), volume);
            js::pushundefined(j);
        },
        "__muenMusicSetVolume",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            const auto result = music::get_pan(music.value());
            js::pushnumber(j, result);
        },
        "__muenMusicGetPan",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            const auto pan = js::tofloat(j, 2);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            music::set_pan(music.value(), pan);
            js::pushundefined(j);
        },
        "__muenMusicSetPan",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            const auto result = music::get_pitch(music.value());
            js::pushnumber(j, result);
        },
        "__muenMusicGetPitch",
        1
    );

    define_global_function(
        j,
        [](js::State *j) -> void {
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            const auto id = js::touint32(j, 1);
            const auto pitch = js::tofloat(j, 2);
            auto music = engine::get_music(*e, id);
            if (!music.has_value()) {
                js::error(j, "Could not inspect invalid music");
            }
            music::set_pitch(music.value(), pitch);
            js::pushundefined(j);
        },
        "__muenMusicSetPitch",
        1
    );
}

} // namespace muen::bindings::music

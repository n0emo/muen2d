#include <engine/plugin.hpp>
#include <plugins/audio/audio.hpp>
#include <plugins/audio/classes/Music.hpp>
#include <plugins/audio/classes/Sound.hpp>

namespace muen::plugins::audio {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "audio",
        .c_modules =
            {
                {"muen:Music", music_class::module(js)},
                {"muen:Sound", sound_class::module(js)},
            },
        .load = []() -> void { audio::init(); },
        .unload = []() -> void { audio::close(); },
        .update = []() -> void {
            for (const auto music : audio::get().musics) {
                if (music::is_playing(*music)) {
                    music::update(*music);
                }
            }
        }
    };
}

} // namespace muen::plugins::audio

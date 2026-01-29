#include <plugins/audio.hpp>

#include <spdlog/spdlog.h>

namespace glint::plugins::audio {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "audio",
        .c_modules =
            {
                {"glint:Music", music_class::module(js)},
                {"glint:Sound", sound_class::module(js)},
            },
        .load = []() -> Result<> {
            engine::audio::init();
            return {};
        },
        .unload = []() -> Result<> {
            engine::audio::close();
            return {};
        },
        .update = []() -> Result<> {
            for (const auto music : engine::audio::get().musics) {
                if (engine::audio::music::is_playing(*music)) {
                    engine::audio::music::update(*music);
                }
            }
            return {};
        }
    };
}

} // namespace glint::plugins::audio

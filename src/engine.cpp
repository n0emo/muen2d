#include "engine.hpp"

#include <spdlog/spdlog.h>

#include "audio.hpp"
#include "js.hpp"
#include "bindings.hpp"
#include "defer.hpp"
#include "window.hpp"

namespace muen::engine {

constexpr char CAMERA_JS[] = {
#include "Camera.js.h" 
};
constexpr char COLOR_JS[] = {
#include "Color.js.h" 
};
constexpr char CONSOLE_JS[] = {
#include "console.js.h" 
};
constexpr char GRAPHICS_JS[] = {
#include "graphics.js.h" 
};
constexpr char MUSIC_JS[] = {
#include "Music.js.h"
};
constexpr char RECTANGLE_JS[] = {
#include "Rectangle.js.h" 
};
constexpr char SCREEN_JS[] = {
#include "screen.js.h" 
};
constexpr char SOUND_JS[] = {
#include "Sound.js.h"
};
constexpr char VECTOR2_JS[] = {
#include "Vector2.js.h" 
};
constexpr char TEXTURE_JS[] = {
#include "Texture.js.h" 
};

auto read_config(Engine& self) -> Config;

auto create() -> Engine {
    auto js = js::newstate(nullptr, nullptr, js::STRICT);
    return Engine {.js = js};
}

void destroy(Engine& self) {
    js::freestate(self.js);
}

auto run(Engine& self, const char *path) -> int {
    window::setup();

    self.root_path = path;
    self.muen_modules = {
        {"muen/Camera.js", CAMERA_JS},
        {"muen/Color.js", COLOR_JS},
        {"muen/console.js", CONSOLE_JS},
        {"muen/Music.js", MUSIC_JS},
        {"muen/Rectangle.js", RECTANGLE_JS},
        {"muen/Sound.js", SOUND_JS},
        {"muen/Vector2.js", VECTOR2_JS},
        {"muen/graphics.js", GRAPHICS_JS},
        {"muen/screen.js", SCREEN_JS},
        {"muen/Texture.js", TEXTURE_JS},
    };

    js::setcontext(self.js, &self);

    bindings::define(self.js);

    auto game_path = std::string {path};
    if (game_path[game_path.size() - 1] != '/') {
        game_path += '/';
    }
    game_path += "Game.js";

    Config config;
    try {
        mujs_catch(self.js);

        js::pushliteral(
            self.js,
            R"(
            var Game = require("Game");

            if (!Game) {
                throw Error("Game module does not export Game constructor. Forgot `module.exports = Game`?");
            }

            var game = new Game();
            )"
        );
        js::eval(self.js);
        js::pop(self.js, 1);
        js::endtry(self.js);

        config = read_config(self);
    } catch (const js::Exception& e) {
        spdlog::error("Error initializing game: {}", e.what());
        return 1;
    }

    auto w = window::create(
        window::Config {
            .width = config.width,
            .height = config.height,
            .fps = config.fps,
            .title = config.title,
        }
    );
    defer(window::close(w));

    auto a = audio::create();
    defer(audio::close(a));

    // TODO: free resources

    try {
        mujs_catch(self.js);

        js::pushliteral(
            self.js,
            R"(
            if (game.load) {
                game.load();
            })"
        );
        js::eval(self.js);
        js::pop(self.js, 1);

        while (!window::should_close(w)) {
            for (auto [id, music] : self.musics) {
                if (audio::music::is_playing(music)) {
                    audio::music::update(music);
                }
            }

            js::getglobal(self.js, "game");
            js::getproperty(self.js, -1, "update");
            js::rot2(self.js);
            js::call(self.js, 0);
            js::pop(self.js, 1);

            window::begin_drawing(w);
            window::clear(w);

            js::getglobal(self.js, "game");
            js::getproperty(self.js, -1, "draw");
            js::rot2(self.js);
            js::call(self.js, 0);
            js::pop(self.js, 1);

            window::draw_fps(w);
            window::end_drawing(w);
        }
        js::endtry(self.js);
    } catch (js::Exception& e) {
        spdlog::error("Error running game: {}\n", e.what());
        js::dump(self.js);
        return 1;
    }

    return 0;
}

auto is_audio_ready(Engine& self) -> bool {
    return ::IsAudioDeviceReady();
}

auto load_sound(Engine& self, std::string path) -> std::expected<uint32_t, std::string> {
    static std::atomic_uint32_t id_counter = 1;

    if (auto search = self.sounds_cache.find(path); search != self.sounds_cache.end()) {
        return search->second;
    }

    auto id = id_counter++;
    auto sound = audio::sound::load(path);
    if (!sound.has_value()) {
        return std::unexpected(sound.error());
    }

    self.sounds.insert({id, sound.value()});
    self.sounds_cache.insert({std::move(path), id});
    return id;
}

auto unload_sound(Engine& self, uint32_t id) -> void {
    if (auto it = self.sounds.find(id); it != self.sounds.end()) {
        auto sound = it->second;
        self.sounds.erase(it);
        audio::sound::unload(sound);
    }
}

auto get_sound(Engine& self, uint32_t id) -> std::optional<audio::Sound> {
    if (auto it = self.sounds.find(id); it != self.sounds.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

auto load_music(Engine& self, const std::string& filename) -> std::expected<uint32_t, std::string> {
    static std::atomic_uint32_t id_counter = 1;

    if (auto search = self.musics_cache.find(filename); search != self.musics_cache.end()) {
        return search->second;
    }

    auto id = id_counter++;

    auto path = std::string {self.root_path};
    if (path[path.size() - 1] != '/') {
        path += '/';
    }
    path += filename;

    auto music = audio::music::load(path);
    if (!music.has_value()) {
        return std::unexpected(music.error());
    }

    self.musics.insert({id, music.value()});
    self.musics_cache.insert({filename, id});
    return id;
}

auto unload_music(Engine& self, uint32_t id) -> void {
    if (auto it = self.musics.find(id); it != self.musics.end()) {
        auto music = it->second;
        self.musics.erase(it);
        audio::music::unload(music);
    }
}

auto get_music(Engine& self, uint32_t id) -> std::optional<audio::Music> {
    if (auto it = self.musics.find(id); it != self.musics.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

auto read_config(Engine& self) -> Config {
    mujs_catch(self.js);

    auto config = Config {};
    js::getglobal(self.js, "game");
    if (js::hasproperty(self.js, -1, "config")) {
        if (js::hasproperty(self.js, -1, "fps")) {
            config.fps = js::tointeger(self.js, -1);
            js::pop(self.js, 1);
        }

        if (js::hasproperty(self.js, -1, "width")) {
            config.width = js::tointeger(self.js, -1);
            js::pop(self.js, 1);
        }

        if (js::hasproperty(self.js, -1, "height")) {
            config.height = js::tointeger(self.js, -1);
            js::pop(self.js, 1);
        }

        if (js::hasproperty(self.js, -1, "title")) {
            config.title = js::tostring(self.js, -1);
            js::pop(self.js, 1);
        }

        js::pop(self.js, 1);
    }
    js::pop(self.js, 1);

    js::endtry(self.js);

    return config;
}

} // namespace muen::engine

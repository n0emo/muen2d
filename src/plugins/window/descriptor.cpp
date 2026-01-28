#include <plugins/window.hpp>

namespace glint::plugins::window {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "window",
        .c_modules = {
            {"glint:screen", screen_module(js)},
            {"glint:mouse", mouse::module(js)},
        },
    };
}

}

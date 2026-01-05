#include <plugins/window.hpp>

namespace muen::plugins::window {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "window",
        .c_modules = {
            {"muen:screen", screen_module(js)},
        },
    };
}

}

#include <plugins/graphics.hpp>

namespace muen::plugins::graphics {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "graphics",
        .c_modules = {
            {"muen:Camera", camera::module(js)},
            {"muen:Color", color::module(js)},
            {"muen:Font", font::module(js)},
            {"muen:NPatch", npatch::module(js)},
            {"muen:Texture", texture::module(js)},
            {"muen:graphics", module(js)},
        },
        .draw = []() -> Result<> {
            ClearBackground(BLACK);
            return {};
        }
    };
}

}

#include <plugins/graphics.hpp>

namespace glint::plugins::graphics {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "graphics",
        .c_modules = {
            {"glint:Camera", camera::module(js)},
            {"glint:Color", color::module(js)},
            {"glint:Font", font::module(js)},
            {"glint:NPatch", npatch::module(js)},
            {"glint:Texture", texture::module(js)},
            {"glint:graphics", module(js)},
        },
        .draw = []() -> Result<> {
            ClearBackground(BLACK);
            return {};
        }
    };
}

}

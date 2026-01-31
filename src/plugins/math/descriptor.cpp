#include <plugins/math.hpp>

namespace glint::plugins::math {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "math",
        .c_modules = {
            {"glint:Vector2", vector2::module(js)},
            {"glint:Rectangle", rectangle_module(js)},
        },

    };
}

} // namespace glint::plugins::math

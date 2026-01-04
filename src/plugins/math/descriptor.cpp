#include <plugins/math.hpp>

namespace muen::plugins::math {

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "math",
        .c_modules = {
            {"muen:Vector2", vector2::module(js)},
            {"muen:Rectangle", rectangle::module(js)},
        },

    };
}

} // namespace muen::plugins::math

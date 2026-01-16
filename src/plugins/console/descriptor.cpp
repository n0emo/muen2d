#include <plugins/console.hpp>
#include <engine/plugin.hpp>

namespace muen::plugins::console {


// NOLINTNEXTLINE
static constexpr char CONSOLE_LOAD[] = {
#include "console_load.js.h"
};

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "console",
        .c_modules = {{"muen:console", module(js)}},
        .load = [=]() -> Result<> {
            auto ret = JS_Eval(js, CONSOLE_LOAD, sizeof(CONSOLE_LOAD) - 1, "muen:console/load.js", JS_EVAL_TYPE_MODULE);
            JS_FreeValue(js, ret);
            return {};
        },
    };
}

} // namespace muen::plugins::console

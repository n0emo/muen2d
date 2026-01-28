#include <plugins/console.hpp>
#include <engine/plugin.hpp>

#include <spdlog/spdlog.h>

namespace glint::plugins::console {

// NOLINTNEXTLINE
static constexpr char CONSOLE_LOAD[] = {
#include "console_load.js.h"
};

auto plugin(JSContext *js) -> EnginePlugin {
    return EnginePlugin {
        .name = "console",
        .c_modules = {{"glint:console", module(js)}},
        .load = [=]() -> Result<> {
            spdlog::info("Initializing console");
            auto ret = JS_Eval(js, CONSOLE_LOAD, sizeof(CONSOLE_LOAD) - 1, "glint:console/load.js", JS_EVAL_TYPE_MODULE);
            JS_FreeValue(js, ret);
            return {};
        },
    };
}

} // namespace glint::plugins::console

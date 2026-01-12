#include <plugins/console.hpp>

#include <array>
#include <span>

#include <spdlog/spdlog.h>

#include <defer.hpp>

namespace muen::plugins::console {

using spdlog::level::level_enum;

auto log(JSContext *js, std::span<JSValueConst> args, level_enum level) -> JSValue {
    auto buf = std::stringstream {};
    for (size_t i = 0; i < args.size(); i++) {
        const auto str = JS_ToCString(js, args[i]);
        defer(JS_FreeCString(js, str));

        buf << str;
        if (i < args.size() - 1) {
            buf << " ";
        }
    }

    auto msg = buf.str();
    spdlog::log(level, "{}", msg);

    return JS_UNDEFINED;
}

auto console_trace(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    return log(js, std::span(argv, argc), level_enum::trace);
}

auto console_debug(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    return log(js, std::span(argv, argc), level_enum::debug);
}

auto console_log(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    return log(js, std::span(argv, argc), level_enum::info);
}

auto console_warn(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    return log(js, std::span(argv, argc), level_enum::warn);
}

auto console_error(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    return log(js, std::span(argv, argc), level_enum::err);
}

const static auto FUNCS = std::array {
    ::JSCFunctionListEntry JS_CFUNC_DEF("trace", 0, console_trace),
    ::JSCFunctionListEntry JS_CFUNC_DEF("debug", 0, console_debug),
    ::JSCFunctionListEntry JS_CFUNC_DEF("log", 0, console_log),
    ::JSCFunctionListEntry JS_CFUNC_DEF("warn", 0, console_warn),
    ::JSCFunctionListEntry JS_CFUNC_DEF("error", 0, console_error),
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:console", [](auto js, auto m) -> int {
        auto o = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, o, FUNCS.data(), int{FUNCS.size()});
        JS_SetModuleExport(js, m, "default", o);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");
    return m;
}

}

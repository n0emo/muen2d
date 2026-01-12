#include <plugins/window.hpp>

#include <array>

#include <quickjs.h>
#include <raylib.h>

namespace muen::plugins::window {

static auto get_dt(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewFloat64(js, static_cast<double>(::GetFrameTime()));
}

static auto get_time(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewFloat64(js, static_cast<double>(::GetTime()));
}

static auto get_width(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewFloat64(js, double(::GetScreenWidth()));
}

static auto get_height(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewFloat64(js, double(::GetScreenHeight()));
}

const static auto funcs = std::array{
    ::JSCFunctionListEntry JS_CGETSET_DEF("dt", get_dt, nullptr),
    ::JSCFunctionListEntry JS_CGETSET_DEF("time", get_time, nullptr),
    ::JSCFunctionListEntry JS_CGETSET_DEF("width", get_width, nullptr),
    ::JSCFunctionListEntry JS_CGETSET_DEF("height", get_height, nullptr),
};

auto screen_module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:screen", [](auto js, auto m) -> int {
        auto o = ::JS_NewObject(js);

        ::JS_SetPropertyFunctionList(js, o, funcs.data(), int{funcs.size()});
        ::JS_SetModuleExport(js, m, "default", o);

        return 0;
    });
    ::JS_AddModuleExport(js, m, "default");
    return m;
}


} // namespace muen::plugins::window

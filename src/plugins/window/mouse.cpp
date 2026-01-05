#include <plugins/window.hpp>

#include <cassert>
#include <unordered_map>

#include <raylib.h>

#include <defer.hpp>

static const auto BUTTON_MAP = std::unordered_map<std::string, ::MouseButton> {
    {"left" , MOUSE_BUTTON_LEFT},
    {"right" , MOUSE_BUTTON_RIGHT},
    {"middle" , MOUSE_BUTTON_MIDDLE},
    {"side" , MOUSE_BUTTON_SIDE},
    {"extra" , MOUSE_BUTTON_EXTRA},
    {"forward" , MOUSE_BUTTON_FORWARD},
    {"back" , MOUSE_BUTTON_BACK},
};

static const auto CURSOR_MAP = std::unordered_map<std::string, ::MouseCursor> {
    {"default", MOUSE_CURSOR_DEFAULT},
    {"arrow", MOUSE_CURSOR_ARROW},
    {"ibeam", MOUSE_CURSOR_IBEAM},
    {"crosshair", MOUSE_CURSOR_CROSSHAIR},
    {"pointingHand", MOUSE_CURSOR_POINTING_HAND},
    {"resizeEw", MOUSE_CURSOR_RESIZE_EW},
    {"resizeNs", MOUSE_CURSOR_RESIZE_NS},
    {"resizeNwse", MOUSE_CURSOR_RESIZE_NWSE},
    {"resizeNesw", MOUSE_CURSOR_RESIZE_NESW},
    {"resizeAll", MOUSE_CURSOR_RESIZE_ALL},
    {"notAllowed", MOUSE_CURSOR_NOT_ALLOWED},
};

static auto is_button_pressed(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if( button_cstr == nullptr ) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if ( auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end() ) {
        auto pressed = ::IsMouseButtonPressed(button->second);
        return ::JS_NewBool(js, pressed);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static auto is_button_down(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if( button_cstr == nullptr ) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if ( auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end() ) {
        auto down = ::IsMouseButtonDown(button->second);
        return ::JS_NewBool(js, down);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static auto is_button_released(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if( button_cstr == nullptr ) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if ( auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end() ) {
        auto released = ::IsMouseButtonReleased(button->second);
        return ::JS_NewBool(js, released);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static auto is_button_up(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if( button_cstr == nullptr ) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if ( auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end() ) {
        auto up = ::IsMouseButtonUp(button->second);
        return ::JS_NewBool(js, up);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static const auto FUNCS = std::array{
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonPressed", 1, is_button_pressed),
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonDown", 1, is_button_down),
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonReleased", 1, is_button_released),
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonUp", 1, is_button_up),
};

namespace muen::plugins::window::mouse {
    auto module(::JSContext *js) -> ::JSModuleDef *{
        auto m = ::JS_NewCModule(js, "muen:mouse", [](auto js, auto m) -> int {
            auto o = ::JS_NewObject(js);

            ::JS_SetPropertyFunctionList(js, o, FUNCS.data(), FUNCS.size());

            ::JS_SetModuleExport(js, m, "mouse", o);
            ::JS_SetModuleExport(js, m, "default", o);

            return 0;
        });

        ::JS_AddModuleExport(js, m, "mouse");
        ::JS_AddModuleExport(js, m, "default");

        return m;
    }

}
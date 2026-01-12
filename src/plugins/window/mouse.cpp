#include <plugins/window.hpp>

#include <array>
#include <cassert>
#include <unordered_map>

#include <raylib.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <plugins/math.hpp>

namespace muen::plugins::window::mouse {
static const auto BUTTON_MAP = std::unordered_map<std::string, ::MouseButton> {
    {"left", MOUSE_BUTTON_LEFT},
    {"right", MOUSE_BUTTON_RIGHT},
    {"middle", MOUSE_BUTTON_MIDDLE},
    {"side", MOUSE_BUTTON_SIDE},
    {"extra", MOUSE_BUTTON_EXTRA},
    {"forward", MOUSE_BUTTON_FORWARD},
    {"back", MOUSE_BUTTON_BACK},
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

static auto is_button_pressed(::JSContext *js, ::JSValueConst, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if (button_cstr == nullptr) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if (auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end()) {
        auto pressed = ::IsMouseButtonPressed(button->second);
        return ::JS_NewBool(js, pressed);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static auto is_button_down(::JSContext *js, ::JSValueConst, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if (button_cstr == nullptr) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if (auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end()) {
        auto down = ::IsMouseButtonDown(button->second);
        return ::JS_NewBool(js, down);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static auto is_button_released(::JSContext *js, ::JSValueConst, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if (button_cstr == nullptr) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if (auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end()) {
        auto released = ::IsMouseButtonReleased(button->second);
        return ::JS_NewBool(js, released);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static auto is_button_up(::JSContext *js, ::JSValueConst, int argc, ::JSValueConst *argv) -> ::JSValue {
    assert(argc == 1);

    auto button_cstr = ::JS_ToCString(js, argv[0]);
    if (button_cstr == nullptr) {
        return ::JS_ThrowTypeError(js, "Button must be string");
    }
    defer(::JS_FreeCString(js, button_cstr));

    if (auto button = BUTTON_MAP.find(button_cstr); button != BUTTON_MAP.end()) {
        auto up = ::IsMouseButtonUp(button->second);
        return ::JS_NewBool(js, up);
    } else {
        return ::JS_ThrowTypeError(js, "Unknown mouse button `%s`", button_cstr);
    }
}

static auto get_x(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewInt32(js, ::GetMouseX());
}

static auto get_y(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewInt32(js, ::GetMouseY());
}

static auto set_x(::JSContext *js, ::JSValueConst, ::JSValueConst val) -> ::JSValue {
    auto x = int {};
    auto y = ::GetMouseY();
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Mouse x must be number");
    }
    ::JS_ToInt32(js, &x, val);
    ::SetMousePosition(x, y);

    return ::JS_UNDEFINED;
}

static auto set_y(::JSContext *js, ::JSValueConst, ::JSValueConst val) -> ::JSValue {
    auto x = ::GetMouseX();
    auto y = int {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Mouse y must be number");
    }
    ::JS_ToInt32(js, &y, val);
    ::SetMousePosition(x, y);

    return ::JS_UNDEFINED;
}

static auto get_position(::JSContext *js, ::JSValueConst) -> ::JSValue {
    auto obj = JS_NewObjectClass(js, math::vector2::class_id(js));
    auto vec = new ::Vector2 {::GetMousePosition()};
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto set_position(::JSContext *js, ::JSValueConst, ::JSValueConst val) -> ::JSValue {
    auto obj = math::vector2::from_value(js, val);
    if (!obj.has_value()) {
        return JS_Throw(js, obj.error());
    }

    ::SetMousePosition(int(obj->x), int(obj->y));
    return ::JS_UNDEFINED;
}

static auto get_delta(::JSContext *js, ::JSValueConst) -> ::JSValue {
    auto obj = JS_NewObjectClass(js, math::vector2::class_id(js));
    auto vec = new ::Vector2 {::GetMouseDelta()};
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto set_cursor(::JSContext *js, ::JSValueConst, ::JSValueConst val) -> ::JSValue {
    auto cursor = int {};
    if (!::JS_IsString(val)) {
        return JS_ThrowTypeError(js, "Mouse cursor must be string");
    }
    ::JS_ToInt32(js, &cursor, val);
    ::SetMouseCursor(cursor);

    return ::JS_UNDEFINED;
}

static auto get_visible(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewBool(js, ::IsCursorHidden());
}

static auto set_visible(::JSContext *js, ::JSValueConst, ::JSValueConst val) -> ::JSValue {
    if (!::JS_IsBool(val)) {
        return JS_ThrowTypeError(js, "Mouse cursor must be boolean");
    }

    if (JS_ToBool(js, val)) {
        ::ShowCursor();
    } else {
        ::HideCursor();
    }

    return ::JS_UNDEFINED;
}

static auto set_enable(::JSContext *js, ::JSValueConst, ::JSValueConst val) -> ::JSValue {
    if (!::JS_IsBool(val)) {
        return JS_ThrowTypeError(js, "Mouse cursor must be boolean");
    }

    if (JS_ToBool(js, val)) {
        ::EnableCursor();
    } else {
        ::DisableCursor();
    }

    return ::JS_UNDEFINED;
}

static auto get_is_on_screen(::JSContext *js, ::JSValueConst) -> ::JSValue {
    return ::JS_NewBool(js, ::IsCursorOnScreen());
}

static const auto FUNCS = std::array {
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonPressed", 1, is_button_pressed),
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonDown", 1, is_button_down),
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonReleased", 1, is_button_released),
    ::JSCFunctionListEntry JS_CFUNC_DEF("isButtonUp", 1, is_button_up),
    ::JSCFunctionListEntry JS_CGETSET_DEF("x", get_x, set_x),
    ::JSCFunctionListEntry JS_CGETSET_DEF("y", get_y, set_y),
    ::JSCFunctionListEntry JS_CGETSET_DEF("position", get_position, set_position),
    ::JSCFunctionListEntry JS_CGETSET_DEF("delta", get_delta, nullptr),
    ::JSCFunctionListEntry JS_CGETSET_DEF("cursor", nullptr, set_cursor),
    ::JSCFunctionListEntry JS_CGETSET_DEF("visible", get_visible, set_visible),
    ::JSCFunctionListEntry JS_CGETSET_DEF("enabled", nullptr, set_enable),
    ::JSCFunctionListEntry JS_CGETSET_DEF("isOnScreen", get_is_on_screen, nullptr),

};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:mouse", [](auto js, auto m) -> int {
        auto o = ::JS_NewObject(js);

        ::JS_SetPropertyFunctionList(js, o, FUNCS.data(), int{FUNCS.size()});

        ::JS_SetModuleExport(js, m, "mouse", JS_DupValue(js, o));
        ::JS_SetModuleExport(js, m, "default", o);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "mouse");
    ::JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::window::mouse

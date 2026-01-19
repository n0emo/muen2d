#include <plugins/window.hpp>

#include <array>
#include <cassert>
#include <gsl/gsl>
#include <unordered_map>

#include <raylib.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <plugins/math.hpp>

namespace muen::js {

static const auto BUTTON_MAP = std::unordered_map<std::string, MouseButton> {
    {"left", MOUSE_BUTTON_LEFT},
    {"right", MOUSE_BUTTON_RIGHT},
    {"middle", MOUSE_BUTTON_MIDDLE},
    {"side", MOUSE_BUTTON_SIDE},
    {"extra", MOUSE_BUTTON_EXTRA},
    {"forward", MOUSE_BUTTON_FORWARD},
    {"back", MOUSE_BUTTON_BACK},
};

static const auto CURSOR_MAP = std::unordered_map<std::string, MouseCursor> {
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

template<>
auto try_into<MouseButton>(const Value& val) noexcept -> JSResult<MouseButton> {
    auto str = try_into<std::string>(val);
    if (!str) return Unexpected(str.error());
    if (auto it = BUTTON_MAP.find(*str); it != BUTTON_MAP.end()) {
        return it->second;
    } else {
        return Unexpected(JSError::type_error(val.ctx(), fmt::format("Unknown mouse button `{}`", *str)));
    }
}

template<>
auto try_into<MouseCursor>(const Value& val) noexcept -> JSResult<MouseCursor> {
    auto str = try_into<std::string>(val);
    if (!str) return Unexpected(str.error());
    if (auto it = CURSOR_MAP.find(*str); it != CURSOR_MAP.end()) {
        return it->second;
    } else {
        return Unexpected(JSError::type_error(val.ctx(), fmt::format("Unknown mouse button `{}`", *str)));
    }
}

} // namespace muen::js

namespace muen::plugins::window::mouse {

using namespace gsl;

static auto is_button_pressed(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<MouseButton>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [button] = *args;
    return JS_NewBool(js, IsMouseButtonPressed(button));
}

static auto is_button_down(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<MouseButton>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [button] = *args;
    return JS_NewBool(js, IsMouseButtonDown(button));
}

static auto is_button_released(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<MouseButton>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [button] = *args;
    return JS_NewBool(js, IsMouseButtonReleased(button));
}

static auto is_button_up(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<MouseButton>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [button] = *args;
    return JS_NewBool(js, IsMouseButtonUp(button));
}

static auto get_x(JSContext *js, JSValueConst) -> JSValue {
    return JS_NewInt32(js, GetMouseX());
}

static auto get_y(JSContext *js, JSValueConst) -> JSValue {
    return JS_NewInt32(js, GetMouseY());
}

static auto set_x(JSContext *js, JSValueConst, JSValueConst val) -> JSValue {
    auto x = js::try_into<int>(js::borrow(js, val));
    if (!x) return jsthrow(x.error());
    auto y = GetMouseY();
    SetMousePosition(*x, y);
    return JS_UNDEFINED;
}

static auto set_y(JSContext *js, JSValueConst, JSValueConst val) -> JSValue {
    auto y = js::try_into<int>(js::borrow(js, val));
    if (!y) return jsthrow(y.error());
    auto x = GetMouseX();
    SetMousePosition(x, *y);
    return JS_UNDEFINED;
}

static auto get_position(JSContext *js, JSValueConst) -> JSValue {
    auto obj = JS_NewObjectClass(js, js::class_id<&math::vector2::VECTOR2>(js));
    auto vec = owner<Vector2 *>(new Vector2 {GetMousePosition()});
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto set_position(JSContext *js, JSValueConst, JSValueConst val) -> JSValue {
    auto vec = js::try_into<Vector2>(js::borrow(js, val));
    if (!vec) return jsthrow(vec.error());
    SetMousePosition(int(vec->x), int(vec->y));
    return JS_UNDEFINED;
}

static auto get_delta(JSContext *js, JSValueConst) -> JSValue {
    auto obj = JS_NewObjectClass(js, js::class_id<&math::vector2::VECTOR2>(js));
    auto vec = owner<Vector2 *>(new Vector2 {::GetMouseDelta()});
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto set_cursor(JSContext *js, JSValueConst, JSValueConst val) -> JSValue {
    auto cursor = js::try_into<MouseCursor>(js::borrow(js, val));
    if (!cursor) return jsthrow(cursor.error());
    SetMouseCursor(*cursor);
    return JS_UNDEFINED;
}

static auto get_visible(JSContext *js, JSValueConst) -> JSValue {
    return JS_NewBool(js, IsCursorHidden());
}

static auto set_visible(JSContext *js, JSValueConst, JSValueConst val) -> JSValue {
    auto visible = js::try_into<bool>(js::borrow(js, val));
    if (!visible) return jsthrow(visible.error());

    if (*visible) ShowCursor();
    else HideCursor();

    return JS_UNDEFINED;
}

static auto set_enable(JSContext *js, JSValueConst, JSValueConst val) -> JSValue {
    auto enable = js::try_into<bool>(js::borrow(js, val));
    if (!enable) return jsthrow(enable.error());

    if (*enable) EnableCursor();
    else DisableCursor();

    return JS_UNDEFINED;
}

static auto get_is_on_screen(JSContext *js, JSValueConst) -> JSValue {
    return JS_NewBool(js, IsCursorOnScreen());
}

static const auto FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("isButtonPressed", 1, is_button_pressed),
    JSCFunctionListEntry JS_CFUNC_DEF("isButtonDown", 1, is_button_down),
    JSCFunctionListEntry JS_CFUNC_DEF("isButtonReleased", 1, is_button_released),
    JSCFunctionListEntry JS_CFUNC_DEF("isButtonUp", 1, is_button_up),
    JSCFunctionListEntry JS_CGETSET_DEF("x", get_x, set_x),
    JSCFunctionListEntry JS_CGETSET_DEF("y", get_y, set_y),
    JSCFunctionListEntry JS_CGETSET_DEF("position", get_position, set_position),
    JSCFunctionListEntry JS_CGETSET_DEF("delta", get_delta, nullptr),
    JSCFunctionListEntry JS_CGETSET_DEF("cursor", nullptr, set_cursor),
    JSCFunctionListEntry JS_CGETSET_DEF("visible", get_visible, set_visible),
    JSCFunctionListEntry JS_CGETSET_DEF("enabled", nullptr, set_enable),
    JSCFunctionListEntry JS_CGETSET_DEF("isOnScreen", get_is_on_screen, nullptr),

};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:mouse", [](auto js, auto m) -> int {
        auto o = JS_NewObject(js);

        JS_SetPropertyFunctionList(js, o, FUNCS.data(), int {FUNCS.size()});

        JS_SetModuleExport(js, m, "mouse", JS_DupValue(js, o));
        JS_SetModuleExport(js, m, "default", o);

        return 0;
    });

    JS_AddModuleExport(js, m, "mouse");
    JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::window::mouse

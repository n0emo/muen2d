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

static const auto KEY_MAP = std::unordered_map<std::string, KeyboardKey> {
    // Alphanumeric keys
    {"apostrophe", KEY_APOSTROPHE},
    {"comma", KEY_COMMA},
    {"minus", KEY_MINUS},
    {"period", KEY_PERIOD},
    {"slash", KEY_SLASH},
    {"zero", KEY_ZERO},
    {"one", KEY_ONE},
    {"two", KEY_TWO},
    {"three", KEY_THREE},
    {"four", KEY_FOUR},
    {"five", KEY_FIVE},
    {"six", KEY_SIX},
    {"seven", KEY_SEVEN},
    {"eight", KEY_EIGHT},
    {"nine", KEY_NINE},
    {"semicolon", KEY_SEMICOLON},
    {"equal", KEY_EQUAL},
    {"a", KEY_A},
    {"b", KEY_B},
    {"c", KEY_C},
    {"d", KEY_D},
    {"e", KEY_E},
    {"f", KEY_F},
    {"g", KEY_G},
    {"h", KEY_H},
    {"i", KEY_I},
    {"j", KEY_J},
    {"k", KEY_K},
    {"l", KEY_L},
    {"m", KEY_M},
    {"n", KEY_N},
    {"o", KEY_O},
    {"p", KEY_P},
    {"q", KEY_Q},
    {"r", KEY_R},
    {"s", KEY_S},
    {"t", KEY_T},
    {"u", KEY_U},
    {"v", KEY_V},
    {"w", KEY_W},
    {"x", KEY_X},
    {"y", KEY_Y},
    {"z", KEY_Z},
    {"leftBracket", KEY_LEFT_BRACKET},
    {"backslash", KEY_BACKSLASH},
    {"rightBracket", KEY_RIGHT_BRACKET},
    {"grave", KEY_GRAVE},
    // Function keys
    {"space", KEY_SPACE},
    {"escape", KEY_ESCAPE},
    {"enter", KEY_ENTER},
    {"tab", KEY_TAB},
    {"backspace", KEY_BACKSPACE},
    {"insert", KEY_INSERT},
    {"delete", KEY_DELETE},
    {"right", KEY_RIGHT},
    {"left", KEY_LEFT},
    {"down", KEY_DOWN},
    {"up", KEY_UP},
    {"pageUp", KEY_PAGE_UP},
    {"pageDown", KEY_PAGE_DOWN},
    {"home", KEY_HOME},
    {"end", KEY_END},
    {"capsLock", KEY_CAPS_LOCK},
    {"scrollLock", KEY_SCROLL_LOCK},
    {"numLock", KEY_NUM_LOCK},
    {"printScreen", KEY_PRINT_SCREEN},
    {"pause", KEY_PAUSE},
    {"f1", KEY_F1},
    {"f2", KEY_F2},
    {"f3", KEY_F3},
    {"f4", KEY_F4},
    {"f5", KEY_F5},
    {"f6", KEY_F6},
    {"f7", KEY_F7},
    {"f8", KEY_F8},
    {"f9", KEY_F9},
    {"f10", KEY_F10},
    {"f11", KEY_F11},
    {"f12", KEY_F12},
    {"leftShift", KEY_LEFT_SHIFT},
    {"leftControl", KEY_LEFT_CONTROL},
    {"leftAlt", KEY_LEFT_ALT},
    {"leftSuper", KEY_LEFT_SUPER},
    {"rightShift", KEY_RIGHT_SHIFT},
    {"rightControl", KEY_RIGHT_CONTROL},
    {"rightAlt", KEY_RIGHT_ALT},
    {"rightSuper", KEY_RIGHT_SUPER},
    {"kbMenu", KEY_KB_MENU},
    // Keypad keys
    {"kp0", KEY_KP_0},
    {"kp1", KEY_KP_1},
    {"kp2", KEY_KP_2},
    {"kp3", KEY_KP_3},
    {"kp4", KEY_KP_4},
    {"kp5", KEY_KP_5},
    {"kp6", KEY_KP_6},
    {"kp7", KEY_KP_7},
    {"kp8", KEY_KP_8},
    {"kp9", KEY_KP_9},
    {"kpDecimal", KEY_KP_DECIMAL},
    {"kpDivide", KEY_KP_DIVIDE},
    {"kpMultiply", KEY_KP_MULTIPLY},
    {"kpSubtract", KEY_KP_SUBTRACT},
    {"kpAdd", KEY_KP_ADD},
    {"kpEnter", KEY_KP_ENTER},
    {"kpEqual", KEY_KP_EQUAL},
    // Android key buttons
    {"back", KEY_BACK},
    {"menu", KEY_MENU},
    {"volumeUp", KEY_VOLUME_UP},
    {"volumeDown", KEY_VOLUME_DOWN},

};

template<>
auto try_into<KeyboardKey>(const Value& val) noexcept -> JSResult<KeyboardKey> {
    auto str = try_into<std::string>(val);
    if (!str) return Unexpected(str.error());
    if (auto it = KEY_MAP.find(*str); it != KEY_MAP.end()) {
        return it->second;
    } else {
        return Unexpected(JSError::type_error(val.ctx(), fmt::format("Unknown keyboard key `{}`", *str)));
    }
}

} // namespace muen::js

namespace muen::plugins::window::keyboard {

using namespace gsl;

static auto is_key_pressed(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<KeyboardKey>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [key] = *args;
    return JS_NewBool(js, IsKeyPressed(key));
}

static auto is_key_pressed_repeat(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<KeyboardKey>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [key] = *args;
    return JS_NewBool(js, IsKeyPressedRepeat(key));
}

static auto is_key_down(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<KeyboardKey>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [key] = *args;
    return JS_NewBool(js, IsKeyDown(key));
}

static auto is_key_released(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<KeyboardKey>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [key] = *args;
    return JS_NewBool(js, IsKeyReleased(key));
}

static auto is_key_up(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<KeyboardKey>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [key] = *args;
    return JS_NewBool(js, IsKeyUp(key));
}

static const auto FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("isKeyPressed", 1, is_key_pressed),
    JSCFunctionListEntry JS_CFUNC_DEF("isKeyPressedRepeat", 1, is_key_pressed_repeat),
    JSCFunctionListEntry JS_CFUNC_DEF("isKeyDown", 1, is_key_down),
    JSCFunctionListEntry JS_CFUNC_DEF("isKeyReleased", 1, is_key_released),
    JSCFunctionListEntry JS_CFUNC_DEF("isKeyUp", 1, is_key_up)
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:keyboard", [](auto js, auto m) -> int {
        auto o = JS_NewObject(js);

        JS_SetPropertyFunctionList(js, o, FUNCS.data(), int {FUNCS.size()});

        JS_SetModuleExport(js, m, "keyboard", JS_DupValue(js, o));
        JS_SetModuleExport(js, m, "default", o);

        return 0;
    });

    JS_AddModuleExport(js, m, "keyboard");
    JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::window::keyboard

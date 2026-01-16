#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <format>
#include <string>
#include <charconv>

#include <raylib.h>

#include <defer.hpp>
#include <error.hpp>

namespace js {

using muen::Err;

template<>
auto try_as<Color>(JSContext *js, JSValueConst val) -> std::expected<Color, JSValue> {
    using namespace muen::plugins;

    const auto id = class_id<&graphics::color::CLASS>(js);
    if (JS_GetClassID(val) == id) {
        const auto ptr = static_cast<::Color *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    auto c = Color {};

    if (auto r = try_get_property<unsigned char>(js, val, "r")) c.r = *r;
    else return Err(r.error());
    if (auto g = try_get_property<unsigned char>(js, val, "g")) c.g = *g;
    else return Err(g.error());
    if (auto b = try_get_property<unsigned char>(js, val, "b")) c.b = *b;
    else return Err(b.error());
    if (auto a = try_get_property<unsigned char>(js, val, "a")) c.a = *a;
    else return Err(a.error());

    return c;
}
} // namespace js

namespace muen::plugins::graphics::color {

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValue val) -> void;
static auto get_r(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_g(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_b(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_a(JSContext *js, JSValueConst this_val) -> JSValue;
static auto set_r(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_g(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_b(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_a(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto to_string(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue;
static auto from_hex(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue;

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("r", get_r, set_r),
    JSCFunctionListEntry JS_CGETSET_DEF("g", get_g, set_g),
    JSCFunctionListEntry JS_CGETSET_DEF("b", get_b, set_b),
    JSCFunctionListEntry JS_CGETSET_DEF("a", get_a, set_a),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

static const auto STATIC_FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("fromHex", 1, from_hex),
};

extern const JSClassDef CLASS = {
    .class_name = "Color",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Color", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&CLASS>(js), &CLASS);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&CLASS>(js), proto);

        JSValue ctor = JS_NewCFunction2(js, constructor, "Color", 4, ::JS_CFUNC_constructor, 0);
        JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int {STATIC_FUNCS.size()});
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(Color col) -> std::string {
    return std::format("Color {{ r: {}, g: {}, b: {}, a: {} }}", col.r, col.g, col.b, col.a);
}

auto pointer_from_value(::JSContext *js, ::JSValueConst val) -> ::Color * {
    return static_cast<Color *>(JS_GetOpaque(val, js::class_id<&CLASS>(js)));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 3 && argc != 4) {
        return JS_ThrowTypeError(js, "Expected 3-4 arguments, got %d", argc);
    }

    for (int i = 0; i < argc; i++) {
        if (!::JS_IsNumber(argv[i])) {
            return JS_ThrowTypeError(js, "Color r, g, b, a must be numbers");
        }
    }

    auto r = int32_t {}, g = int32_t {}, b = int32_t {}, a = int32_t {};
    JS_ToInt32(js, &r, argv[0]);
    JS_ToInt32(js, &g, argv[1]);
    JS_ToInt32(js, &b, argv[2]);
    if (argc == 4) {
        JS_ToInt32(js, &a, argv[3]);
    } else {
        a = 255;
    }

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&CLASS>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto col = new Color {
        .r = static_cast<unsigned char>(r),
        .g = static_cast<unsigned char>(g),
        .b = static_cast<unsigned char>(b),
        .a = static_cast<unsigned char>(a),
    };
    JS_SetOpaque(obj, col);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) -> void {
    auto ptr = static_cast<Color *>(JS_GetOpaque(val, js::class_id<&CLASS>(rt)));
    delete ptr;
}

static auto from_hex(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<std::string>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    auto [hex] = *args;

    if (!hex.starts_with('#')) {
        return JS_ThrowTypeError(js, "Hex code must start with '#'");
    }
    hex = hex.substr(1);

    if (hex.length() != 6 && hex.length() != 8) {
        return JS_ThrowTypeError(js, "Hex code must be in format #rrggbb or #rrggbbaa");
    }

    auto parse_hex_pair = [](const std::string& str, size_t pos) -> unsigned char {
        unsigned int value = 0;
        std::from_chars(str.data() + pos, str.data() + pos + 2, value, 16);
        return static_cast<unsigned char>(value);
    };

    unsigned char r = parse_hex_pair(hex, 0);
    unsigned char g = parse_hex_pair(hex, 2);
    unsigned char b = parse_hex_pair(hex, 4);
    unsigned char a = 255;
    if (hex.length() == 8) {
        a = parse_hex_pair(hex, 6);
    }

    auto obj = JS_NewObjectClass(js, js::class_id<&CLASS>(js));
    auto col = new Color {.r = r, .g = g, .b = b, .a = a};
    JS_SetOpaque(obj, col);
    return obj;
}

static auto get_r(JSContext *js, JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return JS_NewInt32(js, col->r);
}

static auto get_g(JSContext *js, JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return JS_NewInt32(js, col->g);
}

static auto get_b(JSContext *js, JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return JS_NewInt32(js, col->b);
}

static auto get_a(JSContext *js, JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return JS_NewInt32(js, col->a);
}

static auto set_r(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    const auto r = js::try_as<unsigned char>(js, val);
    if (!r) return JS_Throw(js, r.error());
    auto col = pointer_from_value(js, this_val);
    col->r = *r;
    return ::JS_UNDEFINED;
}

static auto set_g(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    const auto g = js::try_as<unsigned char>(js, val);
    if (!g) return JS_Throw(js, g.error());
    auto col = pointer_from_value(js, this_val);
    col->g = *g;
    return ::JS_UNDEFINED;
}

static auto set_b(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    const auto b = js::try_as<unsigned char>(js, val);
    if (!b) return JS_Throw(js, b.error());
    auto col = pointer_from_value(js, this_val);
    col->b = *b;
    return ::JS_UNDEFINED;
}

static auto set_a(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    const auto a = js::try_as<unsigned char>(js, val);
    if (!a) return JS_Throw(js, a.error());
    auto col = pointer_from_value(js, this_val);
    col->a = *a;
    return ::JS_UNDEFINED;
}

static auto to_string(::JSContext *js, JSValueConst this_val, int, JSValueConst *) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    const auto str = to_string(*col);
    return JS_NewString(js, str.c_str());
}

} // namespace muen::plugins::graphics::color

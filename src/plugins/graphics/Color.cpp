#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <format>
#include <string>
#include <charconv>

#include <raylib.h>

#include <defer.hpp>

namespace muen::plugins::graphics::color {

auto class_id(JSRuntime *rt) -> ::JSClassID {
    static const auto id = [](auto rt) -> ::JSClassID {
        auto id = ::JSClassID {};
        ::JS_NewClassID(rt, &id);
        return id;
    }(rt);
    return id;
}

auto class_id(JSContext *js) -> ::JSClassID {
    return class_id(JS_GetRuntime(js));
}

auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Color, ::JSValue> {
    const auto id = class_id(js);
    if (JS_GetClassID(val) == id) {
        const auto ptr = static_cast<::Color *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    auto c = ::Color {};
    auto prop = ::JSValue {};
    auto temp = int32_t {};

    prop = ::JS_GetPropertyStr(js, val, "r");
    ::JS_ToInt32(js, &temp, prop);
    c.r = static_cast<unsigned char>(temp);
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "g");
    ::JS_ToInt32(js, &temp, prop);
    c.g = static_cast<unsigned char>(temp);
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "b");
    ::JS_ToInt32(js, &temp, prop);
    c.b = static_cast<unsigned char>(temp);
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "a");
    ::JS_ToInt32(js, &temp, prop);
    c.a = static_cast<unsigned char>(temp);
    ::JS_FreeValue(js, prop);

    if (::JS_HasException(js)) {
        return std::unexpected(::JS_GetException(js));
    } else {
        return c;
    }
}

auto pointer_from_value(::JSContext *js, ::JSValueConst val) -> ::Color * {
    return static_cast<Color *>(JS_GetOpaque(val, class_id(js)));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 3 && argc != 4) {
        return JS_ThrowTypeError(js, "Color constructor expects 3-4 arguments, but %d were provided", argc);
    }

    for (int i = 0; i < argc; i++) {
        if (!::JS_IsNumber(argv[i])) {
            return JS_ThrowTypeError(js, "Color r, g, b, a must be numbers");
        }
    }

    auto r = int32_t {}, g = int32_t {}, b = int32_t {}, a = int32_t {};
    ::JS_ToInt32(js, &r, argv[0]);
    ::JS_ToInt32(js, &g, argv[1]);
    ::JS_ToInt32(js, &b, argv[2]);
    if (argc == 4) {
        ::JS_ToInt32(js, &a, argv[3]);
    } else {
        a = 255;
    }

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, class_id(js));
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

static auto finalizer(JSRuntime *rt, JSValue val) {
    auto ptr = static_cast<Color *>(JS_GetOpaque(val, class_id(rt)));
    delete ptr;
}

static auto from_hex(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    if (argc != 1) {
        return JS_ThrowTypeError(js, "Color.fromHex expects 1 argument, but %d were provided", argc);
    }

    const char *hex_str = JS_ToCString(js, argv[0]);
    if (!hex_str) {
        return JS_ThrowTypeError(js, "Color.fromHex expects a string argument");
    }
    defer(JS_FreeCString(js, hex_str));

    std::string hex(hex_str);

    if (!hex.empty() && hex[0] == '#') {
        hex = hex.substr(1);
    }

    if (hex.length() != 6 && hex.length() != 8) {
        return JS_ThrowTypeError(js, "Color.fromHex expects format #rrggbb or #rrggbbaa");
    }

    unsigned int r = 0, g = 0, b = 0, a = 255;

    auto parse_hex_pair = [](const std::string& str, size_t pos) -> unsigned int {
        unsigned int value = 0;
        std::from_chars(str.data() + pos, str.data() + pos + 2, value, 16);
        return value;
    };

    r = parse_hex_pair(hex, 0);
    g = parse_hex_pair(hex, 2);
    b = parse_hex_pair(hex, 4);

    if (hex.length() == 8) {
        a = parse_hex_pair(hex, 6);
    }

    auto obj = JS_NewObjectClass(js, class_id(js));
    auto col = new Color {
        .r = static_cast<unsigned char>(r),
        .g = static_cast<unsigned char>(g),
        .b = static_cast<unsigned char>(b),
        .a = static_cast<unsigned char>(a),
    };
    ::JS_SetOpaque(obj, col);
    return obj;
}

static auto get_r(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, col->r);
}

static auto get_g(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, col->g);
}

static auto get_b(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, col->b);
}

static auto get_a(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, col->a);
}

static auto set_r(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto r = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Color r must be number");
    }
    ::JS_ToInt32(js, &r, val);
    auto col = pointer_from_value(js, this_val);
    col->r = static_cast<unsigned char>(r);

    return ::JS_UNDEFINED;
}

static auto set_g(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto g = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Color g must be number");
    }
    ::JS_ToInt32(js, &g, val);
    auto col = pointer_from_value(js, this_val);
    col->g = static_cast<unsigned char>(g);

    return ::JS_UNDEFINED;
}

static auto set_b(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto b = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Color b must be number");
    }
    ::JS_ToInt32(js, &b, val);
    auto col = pointer_from_value(js, this_val);
    col->b = static_cast<unsigned char>(b);

    return ::JS_UNDEFINED;
}

static auto set_a(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto a = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Color a must be number");
    }
    ::JS_ToInt32(js, &a, val);
    auto col = pointer_from_value(js, this_val);
    col->a = static_cast<unsigned char>(a);

    return ::JS_UNDEFINED;
}

static auto to_string(::JSContext *js, JSValueConst this_val, int, JSValueConst *) -> ::JSValue {
    auto col = pointer_from_value(js, this_val);
    const auto str = to_string(*col);
    return JS_NewString(js, str.c_str());
}

static const auto PROTO_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CGETSET_DEF("r", get_r, set_r),
    ::JSCFunctionListEntry JS_CGETSET_DEF("g", get_g, set_g),
    ::JSCFunctionListEntry JS_CGETSET_DEF("b", get_b, set_b),
    ::JSCFunctionListEntry JS_CGETSET_DEF("a", get_a, set_a),
    ::JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

static const auto STATIC_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CFUNC_DEF("fromHex", 1, from_hex),
};

static const auto COLOR_CLASS = ::JSClassDef {
    .class_name = "Color",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:Color", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), class_id(js), &COLOR_CLASS);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int{PROTO_FUNCS.size()});
        ::JS_SetClassProto(js, class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, constructor, "Color", 4, ::JS_CFUNC_constructor, 0);
        ::JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int{STATIC_FUNCS.size()});
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(Color col) -> std::string {
    return std::format("Color {{ r: {}, g: {}, b: {}, a: {} }}", col.r, col.g, col.b, col.a);
}

} // namespace muen::plugins::graphics::color

namespace js {

template<>
auto try_as<::Color>(::JSContext *js, ::JSValueConst value) -> std::expected<::Color, ::JSValue> {
    return muen::plugins::graphics::color::from_value(js, value);
}

} // namespace js

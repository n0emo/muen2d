#include <plugins/graphics.hpp>

#include <array>
#include <charconv>
#include <expected>
#include <gsl/gsl>
#include <string>

#include <fmt/format.h>
#include <raylib.h>

#include <defer.hpp>
#include <error.hpp>

namespace muen::js {

template<>
auto try_into<Color>(const Value& val) noexcept -> JSResult<Color> {
    if (auto r = plugins::graphics::color::ColorClassData::from_value(val)) return (*r)->color;

    auto c = Color {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<unsigned char>("r")) c.r = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<unsigned char>("g")) c.g = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<unsigned char>("b")) c.b = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<unsigned char>("a")) c.a = *v;
    else return Unexpected(v.error());

    return c;
}
} // namespace muen::js

namespace muen::plugins::graphics::color {

auto ColorClassData::from_value(const Value& val) -> JSResult<ColorClassData *> {
    const auto data = static_cast<ColorClassData *>(JS_GetOpaque(val.cget(), class_id<&COLOR>(val.ctx())));
    if (data == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of Color"));
    return data;
}

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

extern const JSClassDef COLOR = {
    .class_name = "Color",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Color", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&COLOR>(js), &COLOR);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&COLOR>(js), proto);

        JSValue ctor = JS_NewCFunction2(js, constructor, "Color", 4, JS_CFUNC_constructor, 0);
        JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int {STATIC_FUNCS.size()});
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 3 && argc != 4) {
        return JS_ThrowTypeError(js, "Expected 3-4 arguments, got %d", argc);
    }

    for (int i = 0; i < argc; i++) {
        if (!JS_IsNumber(argv[i])) {
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

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&COLOR>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto col = owner<Color *>(new Color {
        .r = static_cast<unsigned char>(r),
        .g = static_cast<unsigned char>(g),
        .b = static_cast<unsigned char>(b),
        .a = static_cast<unsigned char>(a),
    });
    JS_SetOpaque(obj, col);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) -> void {
    auto ptr = owner<ColorClassData *>(JS_GetOpaque(val, js::class_id<&COLOR>(rt)));
    delete ptr;
}

static auto from_hex(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    const auto args = js::unpack_args<std::string>(js, argc, argv);
    if (!args) return jsthrow(args.error());
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

    auto obj = JS_NewObjectClass(js, js::class_id<&COLOR>(js));
    auto col = owner<Color *>(new Color {.r = r, .g = g, .b = b, .a = a});
    JS_SetOpaque(obj, col);
    return obj;
}

static auto get_r(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewNumber(js, (*data)->color.r);
}

static auto get_g(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewNumber(js, (*data)->color.g);
}

static auto get_b(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewNumber(js, (*data)->color.b);
}

static auto get_a(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewNumber(js, (*data)->color.a);
}

static auto set_r(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto r = js::try_into<unsigned char>(js::Value::borrowed(js, val));
    if (!r) return jsthrow(r.error());
    (*data)->color.r = *r;
    return JS_UNDEFINED;
}

static auto set_g(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto g = js::try_into<unsigned char>(js::Value::borrowed(js, val));
    if (!g) return jsthrow(g.error());
    (*data)->color.g = *g;
    return JS_UNDEFINED;
}

static auto set_b(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto b = js::try_into<unsigned char>(js::Value::borrowed(js, val));
    if (!b) return jsthrow(b.error());
    (*data)->color.b = *b;
    return JS_UNDEFINED;
}

static auto set_a(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = ColorClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto a = js::try_into<unsigned char>(js::borrow(js, val));
    if (!a) return jsthrow(a.error());
    (*data)->color.a = *a;
    return JS_UNDEFINED;
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto col = js::try_into<Color>(js::borrow(js, this_val));
    if (!col) return jsthrow(col.error());
    const auto str = fmt::format("{}", *col);
    return JS_NewString(js, str.c_str());
}

} // namespace muen::plugins::graphics::color

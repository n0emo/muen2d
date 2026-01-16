#include <plugins/math.hpp>

#include <array>
#include <expected>
#include <format>

#include <raylib.h>

#include <jsutil.hpp>
#include <defer.hpp>

namespace muen::plugins::math::vector2 {

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

auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Vector2, ::JSValue> {
    const auto id = class_id(js);
    if (JS_GetClassID(val) == class_id(js)) {
        auto ptr = static_cast<::Vector2 *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    auto v = ::Vector2 {};
    auto prop = ::JSValue {};
    auto temp = double {};

    prop = ::JS_GetPropertyStr(js, val, "x");
    defer(::JS_FreeValue(js, prop));
    ::JS_ToFloat64(js, &temp, prop);
    v.x = static_cast<float>(temp);

    prop = ::JS_GetPropertyStr(js, val, "y");
    defer(::JS_FreeValue(js, prop));
    ::JS_ToFloat64(js, &temp, prop);
    v.y = static_cast<float>(temp);

    if (JS_HasException(js)) {
        return Err(JS_GetException(js));
    } else {
        return v;
    }
}

auto pointer_from_value(::JSContext *js, ::JSValueConst val) -> ::Vector2 * {
    const auto id = class_id(js);
    return static_cast<::Vector2 *>(JS_GetOpaque(val, id));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 2) {
        return JS_ThrowTypeError(js, "Vector2 constructor expects 2 arguments but %d were provided", argc);
    }

    auto x = double {}, y = double {};
    if (!::JS_IsNumber(argv[0]) || !::JS_IsNumber(argv[1])) {
        return JS_ThrowTypeError(js, "Vector2 x and y must be number");
    }

    ::JS_ToFloat64(js, &x, argv[0]);
    ::JS_ToFloat64(js, &y, argv[1]);

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

    auto vec = new Vector2 {.x = float(x), .y = float(y)};
    JS_SetOpaque(obj, vec);

    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst val) {
    auto ptr = static_cast<Vector2 *>(JS_GetOpaque(val, class_id(rt)));
    delete ptr;
}

static auto zero(JSContext *js, JSValueConst, int, JSValueConst *) -> JSValue {
    auto obj = JS_NewObjectClass(js, class_id(js));
    auto vec = new Vector2 {.x = 0, .y = 0};
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto get_x(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto v = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, v->x);
}

static auto get_y(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto v = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, v->y);
}

static auto set_x(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto x = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Vector2 x must be number");
    }
    ::JS_ToFloat64(js, &x, val);
    auto vec = pointer_from_value(js, this_val);
    vec->x = float(x);

    return ::JS_UNDEFINED;
}

static auto set_y(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto y = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Vector2 y must be number");
    }
    ::JS_ToFloat64(js, &y, val);
    auto vec = pointer_from_value(js, this_val);
    vec->y = float(y);

    return ::JS_UNDEFINED;
}

static auto object_to_string(::JSContext *js, JSValueConst this_val, int, JSValueConst *) -> ::JSValue {
    auto vec = pointer_from_value(js, this_val);
    const auto str = to_string(*vec);
    return JS_NewString(js, str.c_str());
}

static const auto PROTO_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CGETSET_DEF("x", get_x, set_x),
    ::JSCFunctionListEntry JS_CGETSET_DEF("y", get_y, set_y),
    ::JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, object_to_string),
};

static const auto STATIC_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CFUNC_DEF("zero", 0, zero),
};

static const auto VECTOR2_CLASS = ::JSClassDef {
    .class_name = "Vector2",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:Vector2", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), class_id(js), &VECTOR2_CLASS);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int{PROTO_FUNCS.size()});
        ::JS_SetClassProto(js, class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, constructor, "Vector2", 1, ::JS_CFUNC_constructor, 0);
        ::JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int{STATIC_FUNCS.size()});
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(::Vector2 vec) -> std::string {
    return std::format("Vector2 {{ x: {}, y: {} }}", vec.x, vec.y);
}

} // namespace muen::plugins::math::vector2

namespace js {

template<>
auto try_as<::Vector2>(::JSContext *js, ::JSValueConst value) -> std::expected<::Vector2, ::JSValue> {
    return muen::plugins::math::vector2::from_value(js, value);
}

} // namespace js

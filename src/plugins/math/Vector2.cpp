#include <plugins/math.hpp>

#include <array>
#include <expected>
#include <gsl/gsl>

#include <fmt/format.h>
#include <raylib.h>

#include <defer.hpp>

namespace muen::js {

template<>
auto try_into<Vector2 *>(const Value& val) noexcept -> JSResult<Vector2 *> {
    const auto id = class_id<&plugins::math::vector2::VECTOR2>(val.ctx());
    const auto ptr = static_cast<::Vector2 *>(JS_GetOpaque(val.cget(), id));
    if (ptr == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of Vector2"));
    return ptr;
}

template<>
auto try_into<Vector2>(const Value& val) noexcept -> JSResult<Vector2> {
    if (auto r = try_into<Vector2 *>(val)) return **r;

    auto vec = Vector2 {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<float>("x")) vec.x = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("y")) vec.y = *v;
    else return Unexpected(v.error());

    return vec;
}

} // namespace muen::js

namespace muen::plugins::math::vector2 {

using namespace gsl;

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 2) {
        return JS_ThrowTypeError(js, "Vector2 constructor expects 2 arguments but %d were provided", argc);
    }

    auto x = double {}, y = double {};
    if (!JS_IsNumber(argv[0]) || !JS_IsNumber(argv[1])) {
        return JS_ThrowTypeError(js, "Vector2 x and y must be number");
    }

    JS_ToFloat64(js, &x, argv[0]);
    JS_ToFloat64(js, &y, argv[1]);

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&VECTOR2>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto vec = owner<Vector2 *>(new Vector2 {.x = float(x), .y = float(y)});
    JS_SetOpaque(obj, vec);

    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst val) {
    auto ptr = owner<Vector2 *>(JS_GetOpaque(val, js::class_id<&VECTOR2>(rt)));
    if (ptr == nullptr) {
        SPDLOG_WARN("Could not free Vector2 because opaque is null");
        return;
    }
    delete ptr;
}

static auto zero(JSContext *js, JSValueConst, int, JSValueConst *) -> JSValue {
    auto obj = JS_NewObjectClass(js, js::class_id<&VECTOR2>(js));
    auto vec = owner<Vector2 *>(new Vector2 {.x = 0, .y = 0});
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto get_x(JSContext *js, JSValueConst this_val) -> JSValue {
    auto v = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!v) return jsthrow(v.error());
    return JS_NewFloat64(js, (*v)->x);
}

static auto get_y(JSContext *js, JSValueConst this_val) -> JSValue {
    auto v = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!v) return jsthrow(v.error());
    return JS_NewFloat64(js, (*v)->y);
}

static auto set_x(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto vec = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!vec) return jsthrow(vec.error());
    auto x = js::try_into<float>(js::borrow(js, val));
    if (!x) return jsthrow(x.error());
    (*vec)->x = *x;
    return JS_UNDEFINED;
}

static auto set_y(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto vec = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!vec) return jsthrow(vec.error());
    auto y = js::try_into<float>(js::borrow(js, val));
    if (!y) return jsthrow(y.error());
    (*vec)->y = *y;
    return JS_UNDEFINED;
}

static auto object_to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto vec = js::try_into<Vector2>(js::borrow(js, this_val));
    if (!vec) return jsthrow(vec.error());
    const auto str = to_string(*vec);
    return JS_NewString(js, str.c_str());
}

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("x", get_x, set_x),
    JSCFunctionListEntry JS_CGETSET_DEF("y", get_y, set_y),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, object_to_string),
};

static const auto STATIC_FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("zero", 0, zero),
};

extern const JSClassDef VECTOR2 = {
    .class_name = "Vector2",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Vector2", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&VECTOR2>(js), &VECTOR2);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&VECTOR2>(js), proto);

        JSValue ctor = JS_NewCFunction2(js, constructor, "Vector2", 1, JS_CFUNC_constructor, 0);
        JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int {STATIC_FUNCS.size()});
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(::Vector2 vec) -> std::string {
    return fmt::format("Vector2 {{ x: {}, y: {} }}", vec.x, vec.y);
}

} // namespace muen::plugins::math::vector2

#include <plugins/math.hpp>

#include <cmath>
#include <expected>
#include <gsl/gsl>

#include <fmt/format.h>
#include <raylib.h>
#include <raymath.h>

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

static inline auto read_vector_from_args(not_null<JSContext *> js, int argc, JSValueConst *argv) noexcept
    -> js::JSResult<Vector2> {
    if (argc == 1) {
        const auto args = js::unpack_args<Vector2>(js, argc, argv);
        if (!args) return Unexpected(args.error());
        const auto [v] = *args;
        return v;
    } else if (argc == 2) {
        const auto args = js::unpack_args<float, float>(js, argc, argv);
        if (!args) return Unexpected(args.error());
        const auto [x, y] = *args;
        return Vector2 {.x = x, .y = y};
    } else {
        return Unexpected(js::JSError::range_error(js, fmt::format("No matching overload for {} arguments", argc)));
    }
}

static inline auto read_vector_and_number_from_args(not_null<JSContext *> js, int argc, JSValueConst *argv) noexcept
    -> js::JSResult<std::tuple<Vector2, float>> {
    if (argc == 2) {
        const auto args = js::unpack_args<Vector2, float>(js, argc, argv);
        if (!args) return Unexpected(args.error());
        return *args;
    } else if (argc == 3) {
        const auto args = js::unpack_args<float, float, float>(js, argc, argv);
        if (!args) return Unexpected(args.error());
        const auto [x, y, a] = *args;
        return std::tuple {Vector2 {.x = x, .y = y}, a};
    } else {
        return Unexpected(js::JSError::range_error(js, fmt::format("No matching overload for {} arguments", argc)));
    }
}

static inline auto read_two_vectors_from_args(not_null<JSContext *> js, int argc, JSValueConst *argv)
    -> js::JSResult<std::tuple<Vector2, Vector2>> {
    if (argc == 2) {
        const auto args = js::unpack_args<Vector2, Vector2>(js, argc, argv);
        if (!args) return Unexpected(args.error());
        return *args;
    } else if (argc == 4) {
        const auto args = js::unpack_args<float, float, float, float>(js, argc, argv);
        if (!args) return Unexpected(args.error());
        const auto [x1, y1, x2, y2] = *args;
        return std::tuple {Vector2 {.x = x1, .y = y1}, Vector2 {.x = x2, .y = y2}};
    } else {
        return Unexpected(js::JSError::range_error(js, fmt::format("No matching overload for {} arguments", argc)));
    }
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    auto v = read_vector_from_args(js, argc, argv);
    if (!v) return jsthrow(v.error());

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

    auto new_vec = owner<Vector2 *>(new Vector2 {*v});
    JS_SetOpaque(obj, new_vec);

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

static auto one(JSContext *js, JSValueConst, int, JSValueConst *) -> JSValue {
    auto obj = JS_NewObjectClass(js, js::class_id<&VECTOR2>(js));
    auto vec = owner<Vector2 *>(new Vector2 {.x = 1, .y = 1});
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

static auto get_length(JSContext *js, JSValueConst this_val) -> JSValue {
    auto v = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!v) return jsthrow(v.error());
    return JS_NewFloat64(js, Vector2Length(**v));
}

static auto get_length_sqr(JSContext *js, JSValueConst this_val) -> JSValue {
    auto v = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!v) return jsthrow(v.error());
    return JS_NewFloat64(js, Vector2LengthSqr(**v));
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

static auto add(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec += *vec2;
    return JS_DupValue(js, this_val);
}

static auto add_num(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    const auto args = js::unpack_args<float>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [num] = *args;
    (*this_vec)->x += num;
    (*this_vec)->y += num;
    return JS_DupValue(js, this_val);
}

static auto sub(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec -= *vec2;
    return JS_DupValue(js, this_val);
}

static auto sub_num(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    const auto args = js::unpack_args<float>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [num] = *args;
    (*this_vec)->x -= num;
    (*this_vec)->y -= num;
    return JS_DupValue(js, this_val);
}

static auto dot(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    return JS_NewFloat64(js, Vector2DotProduct(**this_vec, *vec2));
}

static auto distance(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    return JS_NewFloat64(js, Vector2Distance(**this_vec, *vec2));
}

static auto distance_sqr(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    return JS_NewFloat64(js, Vector2DistanceSqr(**this_vec, *vec2));
}

static auto angle(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    return JS_NewFloat64(js, Vector2Angle(**this_vec, *vec2));
}

static auto line_angle(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto start = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!start) return jsthrow(start.error());
    auto end = read_vector_from_args(js, argc, argv);
    if (!end) return jsthrow(end.error());
    return JS_NewFloat64(js, Vector2LineAngle(**start, *end));
}

static auto scale(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    const auto args = js::unpack_args<float>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [scale] = *args;
    **this_vec = Vector2Scale(**this_vec, scale);
    return JS_DupValue(js, this_val);
}

static auto mul(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec *= *vec2;
    return JS_DupValue(js, this_val);
}

static auto negate(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    **this_vec = Vector2Negate(**this_vec);
    return JS_DupValue(js, this_val);
}

static auto div(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec /= *vec2;
    return JS_DupValue(js, this_val);
}

static auto norm(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    **this_vec = Vector2Normalize(**this_vec);
    return JS_DupValue(js, this_val);
}

static auto lerp(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto args = read_vector_and_number_from_args(js, argc, argv);
    if (!args) return jsthrow(this_vec.error());
    const auto [vec2, amount] = *args;
    **this_vec = Vector2Lerp(**this_vec, vec2, amount);
    return JS_DupValue(js, this_val);
}

static auto reflect(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec = Vector2Reflect(**this_vec, *vec2);
    return JS_DupValue(js, this_val);
}

static auto min(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec = Vector2Min(**this_vec, *vec2);
    return JS_DupValue(js, this_val);
}

static auto max(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec = Vector2Max(**this_vec, *vec2);
    return JS_DupValue(js, this_val);
}

static auto rotate(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    const auto args = js::unpack_args<float>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [angle] = *args;
    **this_vec = Vector2Rotate(**this_vec, angle);
    return JS_DupValue(js, this_val);
}

static auto move_towards(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto args = read_vector_and_number_from_args(js, argc, argv);
    if (!args) return jsthrow(this_vec.error());
    const auto [vec2, amount] = *args;
    **this_vec = Vector2MoveTowards(**this_vec, vec2, amount);
    return JS_DupValue(js, this_val);
}

static auto invert(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    **this_vec = Vector2Invert(**this_vec);
    return JS_DupValue(js, this_val);
}

static auto clamp(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto args = read_two_vectors_from_args(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [min, max] = *args;
    **this_vec = Vector2Clamp(**this_vec, min, max);
    return JS_DupValue(js, this_val);
}

static auto clamp_value(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto args = js::unpack_args<float, float>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [min, max] = *args;
    **this_vec = Vector2ClampValue(**this_vec, min, max);
    return JS_DupValue(js, this_val);
}

static auto equals(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto vec2 = read_vector_from_args(js, argc, argv);
    if (!vec2) return jsthrow(vec2.error());
    **this_vec = Vector2Max(**this_vec, *vec2);
    return JS_NewBool(js, Vector2Equals(**this_vec, *vec2));
}

static auto refract(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());
    auto args = read_vector_and_number_from_args(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [vec2, amount] = *args;
    **this_vec = Vector2Refract(**this_vec, vec2, amount);
    return JS_DupValue(js, this_val);
}

static auto clone(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto this_vec = js::try_into<Vector2 *>(js::borrow(js, this_val));
    if (!this_vec) return jsthrow(this_vec.error());

    auto obj = JS_NewObjectClass(js, js::class_id<&VECTOR2>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_Throw(js, JS_GetException(js));
    }

    auto new_vec = owner<Vector2 *>(new Vector2 {**this_vec});
    JS_SetOpaque(obj, new_vec);
    return JS_DupValue(js, this_val);
}

static auto object_to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto vec = js::try_into<Vector2>(js::borrow(js, this_val));
    if (!vec) return jsthrow(vec.error());
    auto str = fmt::format("{}", *vec);
    return JS_NewStringLen(js, str.c_str(), str.size());
}

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("x", get_x, set_x),
    JSCFunctionListEntry JS_CGETSET_DEF("y", get_y, set_y),
    JSCFunctionListEntry JS_CGETSET_DEF("length", get_length, nullptr),
    JSCFunctionListEntry JS_CGETSET_DEF("lengthSqr", get_length_sqr, nullptr),
    JSCFunctionListEntry JS_CFUNC_DEF("add", 0, add),
    JSCFunctionListEntry JS_CFUNC_DEF("addNum", 1, add_num),
    JSCFunctionListEntry JS_CFUNC_DEF("sub", 0, sub),
    JSCFunctionListEntry JS_CFUNC_DEF("subNum", 1, sub_num),
    JSCFunctionListEntry JS_CFUNC_DEF("dot", 0, dot),
    JSCFunctionListEntry JS_CFUNC_DEF("distance", 0, distance),
    JSCFunctionListEntry JS_CFUNC_DEF("distanceSqr", 0, distance_sqr),
    JSCFunctionListEntry JS_CFUNC_DEF("angle", 0, angle),
    JSCFunctionListEntry JS_CFUNC_DEF("lineAngle", 0, line_angle),
    JSCFunctionListEntry JS_CFUNC_DEF("scale", 1, scale),
    JSCFunctionListEntry JS_CFUNC_DEF("mul", 0, mul),
    JSCFunctionListEntry JS_CFUNC_DEF("negate", 0, negate),
    JSCFunctionListEntry JS_CFUNC_DEF("div", 0, div),
    JSCFunctionListEntry JS_CFUNC_DEF("norm", 0, norm),
    //TODO: JSCFunctionListEntry JS_CFUNC_DEF("transform", 1, transform),
    JSCFunctionListEntry JS_CFUNC_DEF("lerp", 0, lerp),
    JSCFunctionListEntry JS_CFUNC_DEF("reflect", 0, reflect),
    JSCFunctionListEntry JS_CFUNC_DEF("min", 0, min),
    JSCFunctionListEntry JS_CFUNC_DEF("max", 0, max),
    JSCFunctionListEntry JS_CFUNC_DEF("rotate", 1, rotate),
    JSCFunctionListEntry JS_CFUNC_DEF("moveTowards", 0, move_towards),
    JSCFunctionListEntry JS_CFUNC_DEF("invert", 0, invert),
    JSCFunctionListEntry JS_CFUNC_DEF("clamp", 0, clamp),
    JSCFunctionListEntry JS_CFUNC_DEF("clampValue", 2, clamp_value),
    JSCFunctionListEntry JS_CFUNC_DEF("equals", 0, equals),
    JSCFunctionListEntry JS_CFUNC_DEF("refract", 0, refract),
    JSCFunctionListEntry JS_CFUNC_DEF("clone", 0, clone),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, object_to_string),
};

static const auto STATIC_FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("zero", 0, zero),
    JSCFunctionListEntry JS_CFUNC_DEF("one", 0, one),
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

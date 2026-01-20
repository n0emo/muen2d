#include <plugins/math.hpp>

#include <array>
#include <gsl/gsl>

#include <fmt/format.h>
#include <raylib.h>

#include <defer.hpp>

namespace muen::js {

template<>
auto try_into<Rectangle *>(const Value& val) noexcept -> JSResult<Rectangle *> {
    const auto id = class_id<&plugins::math::rectangle::RECTANGLE>(val.ctx());
    if (JS_GetClassID(val.cget()) == id) {
        const auto ptr = static_cast<::Rectangle *>(JS_GetOpaque(val.cget(), id));
        return ptr;
    }

    return Unexpected(JSError::type_error(val.ctx(), "Not an instance of Rectangle"));
}

template<>
auto try_into<Rectangle>(const Value& val) noexcept -> JSResult<Rectangle> {
    if (auto r = try_into<Rectangle *>(val)) return **r;

    auto rec = ::Rectangle {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<float>("x")) rec.x = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("y")) rec.y = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("width")) rec.width = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("height")) rec.height = *v;
    else return Unexpected(v.error());

    return rec;
}

} // namespace muen::js

namespace muen::plugins::math::rectangle {

using namespace gsl;

static auto constructor(JSContext *js, JSValueConst new_target, int argc, JSValueConst *argv) -> JSValue {
    auto args = js::unpack_args<float, float, float, float>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [x, y, width, height] = *args;

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&RECTANGLE>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto rec = owner<Rectangle *>(new Rectangle {.x = x, .y = y, .width = width, .height = height});
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) {
    auto ptr = owner<Rectangle *>(JS_GetOpaque(val, js::class_id<&RECTANGLE>(rt)));
    delete ptr;
}

static auto zero(JSContext *js, JSValueConst, int, JSValueConst *) -> JSValue {
    // TODO: maybe we should get proto from this_value?
    auto obj = JS_NewObjectClass(js, js::class_id<&RECTANGLE>(js));
    auto rec = owner<Rectangle *>(new Rectangle {.x = 0, .y = 0, .width = 0, .height = 0});
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto from_vectors(JSContext *js, JSValueConst, int argc, JSValueConst *argv) -> JSValue {
    // TODO: maybe we should get proto from this_value?

    auto args = js::unpack_args<Vector2, Vector2>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [pos, size] = *args;

    // TODO: maybe we should get proto from this_value?
    auto obj = JS_NewObjectClass(js, js::class_id<&RECTANGLE>(js));
    auto rec = owner<Rectangle *>(new Rectangle {
        .x = pos.x,
        .y = pos.y,
        .width = size.x,
        .height = size.y,
    });
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto get_x(JSContext *js, JSValueConst this_val) -> JSValue {
    auto rec = js::try_into<Rectangle>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    return JS_NewFloat64(js, rec->x);
}

static auto get_y(JSContext *js, JSValueConst this_val) -> JSValue {
    auto rec = js::try_into<Rectangle>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    return JS_NewFloat64(js, rec->y);
}

static auto get_width(JSContext *js, JSValueConst this_val) -> JSValue {
    auto rec = js::try_into<Rectangle>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    return JS_NewFloat64(js, rec->width);
}

static auto get_height(JSContext *js, JSValueConst this_val) -> JSValue {
    auto rec = js::try_into<Rectangle>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    return JS_NewFloat64(js, rec->height);
}

static auto set_x(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto x = try_into<float>(js::Value::borrowed(js, val));
    if (!x) return jsthrow(x.error());
    auto rec = js::try_into<Rectangle *>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    (*rec)->x = *x;

    return JS_UNDEFINED;
}

static auto set_y(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto y = try_into<float>(js::Value::borrowed(js, val));
    if (!y) return jsthrow(y.error());
    auto rec = js::try_into<Rectangle *>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    (*rec)->y = *y;

    return JS_UNDEFINED;
}

static auto set_width(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto width = try_into<float>(js::Value::borrowed(js, val));
    if (!width) return jsthrow(width.error());
    auto rec = js::try_into<Rectangle *>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    (*rec)->width = *width;

    return JS_UNDEFINED;
}

static auto set_height(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto height = try_into<float>(js::Value::borrowed(js, val));
    if (!height) return jsthrow(height.error());
    auto rec = js::try_into<Rectangle *>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    (*rec)->height = *height;

    return JS_UNDEFINED;
}

static auto object_to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto rec = js::try_into<Rectangle>(js::Value::borrowed(js, this_val));
    if (!rec) return jsthrow(rec.error());
    const auto str = to_string(*rec);

    return JS_NewString(js, str.c_str());
}

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("x", get_x, set_x),
    JSCFunctionListEntry JS_CGETSET_DEF("y", get_y, set_y),
    JSCFunctionListEntry JS_CGETSET_DEF("width", get_width, set_width),
    JSCFunctionListEntry JS_CGETSET_DEF("height", get_height, set_height),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, object_to_string),
};

static const auto STATIC_FUNCS = std::array {
    // static const auto STATIC_FUNCS = std::vector<JSCFunctionListEntry>{
    JSCFunctionListEntry JS_CFUNC_DEF("zero", 0, zero),
    JSCFunctionListEntry JS_CFUNC_DEF("fromVectors", 0, from_vectors),
};

extern const JSClassDef RECTANGLE = {
    .class_name = "Vector2",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Rectangle", [](auto js, auto m) -> int {
        const auto id = js::class_id<&RECTANGLE>(js);
        JS_NewClass(JS_GetRuntime(js), id, &RECTANGLE);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, id, proto);

        JSValue ctor = JS_NewCFunction2(js, constructor, "Rectangle", 1, JS_CFUNC_constructor, 0);
        JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int {STATIC_FUNCS.size()});
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(Rectangle rec) -> std::string {
    return fmt::format("Rectangle {{ x: {}, y: {}, width: {}, height: {} }}", rec.x, rec.y, rec.width, rec.height);
}

} // namespace muen::plugins::math::rectangle

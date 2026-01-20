#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <gsl/gsl>

#include <fmt/format.h>
#include <raylib.h>

#include <defer.hpp>
#include <plugins/math.hpp>
#include <error.hpp>

namespace muen::js {

template<>
auto try_into<Camera2D *>(const Value& val) noexcept -> JSResult<Camera2D *> {
    const auto id = class_id<&plugins::graphics::camera::CLASS>(val.ctx());
    auto ptr = static_cast<Camera2D *>(JS_GetOpaque(val.cget(), id));
    if (ptr == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of Camera2D"));
    return ptr;
}

template<>
auto try_into<Camera2D>(const Value& val) noexcept -> JSResult<Camera2D> {
    if (auto r = try_into<Camera2D *>(val)) return **r;

    auto cam = Camera2D {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<Vector2>("offset")) cam.offset = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<Vector2>("target")) cam.target = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("rotation")) cam.rotation = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("zoom")) cam.zoom = *v;
    else return Unexpected(v.error());

    return cam;
}

} // namespace muen::js

namespace muen::plugins::graphics::camera {

using namespace gsl;

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValueConst val) -> void;
static auto default_camera(JSContext *js, JSValueConst, int, JSValueConst *) -> JSValue;
static auto get_offset(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_target(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_rotation(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_zoom(JSContext *js, JSValueConst this_val) -> JSValue;
static auto set_offset(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_target(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_rotation(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_zoom(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto to_string(JSContext *js, JSValueConst, int, JSValueConst *) -> JSValue;

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("offset", get_offset, set_offset),
    JSCFunctionListEntry JS_CGETSET_DEF("target", get_target, set_target),
    JSCFunctionListEntry JS_CGETSET_DEF("rotation", get_rotation, set_rotation),
    JSCFunctionListEntry JS_CGETSET_DEF("zoom", get_zoom, set_zoom),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

static const auto STATIC_FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("default", 0, default_camera),
};

extern const JSClassDef CLASS = {
    .class_name = "Camera",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Camera", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&CLASS>(js), &CLASS);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&CLASS>(js), proto);

        JSValue ctor = ::JS_NewCFunction2(js, constructor, "Camera", 4, ::JS_CFUNC_constructor, 0);
        JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int {STATIC_FUNCS.size()});
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(Camera2D cam) -> std::string {
    return fmt::format(
        "Camera {{ offset: {{{}, {}}}, target: {{{}, {}}}, rotation: {}, zoom: {} }}",
        cam.offset.x,
        cam.offset.y,
        cam.target.x,
        cam.target.y,
        cam.rotation,
        cam.zoom
    );
}

auto pointer_from_value(JSContext *js, JSValueConst val) -> Camera2D * {
    return static_cast<Camera2D *>(JS_GetOpaque(val, js::class_id<&CLASS>(js)));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    SPDLOG_TRACE("Camera.constructor/{}", argc);
    const auto args = js::unpack_args<Vector2, Vector2, float, float>(js, argc, argv);
    if (!args) return js::jsthrow(args.error());
    const auto [offset, target, rotation, zoom] = *args;

    const auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    const auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&CLASS>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    const auto cam = gsl::owner<Camera2D *>(new Camera2D {
        .offset = offset,
        .target = target,
        .rotation = rotation,
        .zoom = zoom,
    });
    JS_SetOpaque(obj, cam);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst val) -> void {
    auto ptr = owner<Camera2D *>(JS_GetOpaque(val, js::class_id<&CLASS>(rt)));
    delete ptr;
}

static auto default_camera(JSContext *js, JSValueConst, int, JSValue *) -> JSValue {
    const auto obj = JS_NewObjectClass(js, js::class_id<&CLASS>(js));
    const auto cam = owner<Vector2 *>(new Camera2D {
        .offset = {0.0f, 0.0f},
        .target = {0.0f, 0.0f},
        .rotation = 0.0f,
        .zoom = 1.0f,
    });
    JS_SetOpaque(obj, cam);
    return obj;
}

static auto get_offset(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto cam = pointer_from_value(js, this_val);
    const auto obj = JS_NewObjectClass(js, js::class_id<&math::vector2::VECTOR2>(js));
    const auto vec = owner<Vector2 *>(new Vector2 {.x = cam->offset.x, .y = cam->offset.y});
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto get_target(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto cam = pointer_from_value(js, this_val);
    const auto obj = JS_NewObjectClass(js, js::class_id<&math::vector2::VECTOR2>(js));
    const auto vec = owner<Vector2 *>(new Vector2 {.x = cam->target.x, .y = cam->target.y});
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto get_rotation(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto cam = pointer_from_value(js, this_val);
    return JS_NewFloat64(js, cam->rotation);
}

static auto get_zoom(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto cam = pointer_from_value(js, this_val);
    return JS_NewFloat64(js, cam->zoom);
}

static auto set_offset(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto offset = js::try_into<Vector2>(js::Value::borrowed(js, val));
    if (!offset) return jsthrow(offset.error());
    auto cam = pointer_from_value(js, this_val);
    cam->offset = *offset;
    return JS_UNDEFINED;
}

static auto set_target(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto target = js::try_into<Vector2>(js::Value::borrowed(js, val));
    if (!target) return jsthrow(target.error());
    auto cam = pointer_from_value(js, this_val);
    cam->target = *target;
    return JS_UNDEFINED;
}

static auto set_rotation(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto rotation = js::try_into<float>(js::Value::borrowed(js, val));
    if (!rotation) return jsthrow(rotation.error());
    auto cam = pointer_from_value(js, this_val);
    cam->rotation = *rotation;
    return JS_UNDEFINED;
}

static auto set_zoom(JSContext *js, JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    const auto zoom = js::try_into<float>(js::Value::borrowed(js, val));
    if (!zoom) return jsthrow(zoom.error());
    auto cam = pointer_from_value(js, this_val);
    cam->zoom = *zoom;
    return JS_UNDEFINED;
}

static auto to_string(JSContext *js, JSValue this_val, int, JSValue *) -> ::JSValue {
    const auto cam = pointer_from_value(js, this_val);
    const auto str = to_string(*cam);
    return JS_NewString(js, str.c_str());
}

} // namespace muen::plugins::graphics::camera

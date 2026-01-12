#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <format>

#include <raylib.h>

#include <defer.hpp>
#include <plugins/math.hpp>

namespace muen::plugins::graphics::camera {

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

auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Camera2D, ::JSValue> {
    const auto id = class_id(js);
    if (JS_GetClassID(val) == id) {
        const auto ptr = static_cast<::Camera2D *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    auto cam = ::Camera2D {};
    auto prop = ::JSValue {};

    prop = ::JS_GetPropertyStr(js, val, "offset");
    auto offset = math::vector2::from_value(js, prop);
    ::JS_FreeValue(js, prop);
    if (!offset.has_value()) {
        return std::unexpected(offset.error());
    }
    cam.offset = *offset;

    prop = ::JS_GetPropertyStr(js, val, "target");
    auto target = math::vector2::from_value(js, prop);
    ::JS_FreeValue(js, prop);
    if (!target.has_value()) {
        return std::unexpected(target.error());
    }
    cam.target = *target;

    prop = ::JS_GetPropertyStr(js, val, "rotation");
    auto temp = double {};
    ::JS_ToFloat64(js, &temp, prop);
    cam.rotation = static_cast<float>(temp);
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "zoom");
    ::JS_ToFloat64(js, &temp, prop);
    cam.zoom = static_cast<float>(temp);
    ::JS_FreeValue(js, prop);

    if (::JS_HasException(js)) {
        return std::unexpected(::JS_GetException(js));
    } else {
        return cam;
    }
}

auto pointer_from_value(::JSContext *js, ::JSValueConst val) -> ::Camera2D * {
    return static_cast<Camera2D *>(JS_GetOpaque(val, class_id(js)));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 4) {
        return JS_ThrowTypeError(js, "Camera constructor expects 4 arguments but %d were provided", argc);
    }

    auto offset = math::vector2::from_value(js, argv[0]);
    if (!offset.has_value()) {
        return offset.error();
    }

    auto target = math::vector2::from_value(js, argv[1]);
    if (!target.has_value()) {
        return target.error();
    }

    if (!::JS_IsNumber(argv[2])) {
        return JS_ThrowTypeError(js, "Camera rotation must be a number");
    }
    auto rotation = double {};
    ::JS_ToFloat64(js, &rotation, argv[2]);

    if (!::JS_IsNumber(argv[3])) {
        return JS_ThrowTypeError(js, "Camera zoom must be a number");
    }
    auto zoom = double {};
    ::JS_ToFloat64(js, &zoom, argv[3]);

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

    auto cam = new Camera2D {
        .offset = *offset,
        .target = *target,
        .rotation = static_cast<float>(rotation),
        .zoom = static_cast<float>(zoom),
    };
    JS_SetOpaque(obj, cam);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst val) {
    auto ptr = static_cast<Camera2D *>(JS_GetOpaque(val, class_id(rt)));
    delete ptr;
}

static auto default_camera(JSContext *js, JSValueConst, int, JSValue *) -> JSValue {
    auto obj = JS_NewObjectClass(js, class_id(js));
    auto cam = new Camera2D {
        .offset = {0.0f, 0.0f},
        .target = {0.0f, 0.0f},
        .rotation = 0.0f,
        .zoom = 1.0f,
    };
    JS_SetOpaque(obj, cam);
    return obj;
}

static auto get_offset(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto cam = pointer_from_value(js, this_val);

    auto obj = JS_NewObjectClass(js, math::vector2::class_id(js));
    auto vec = new Vector2 {.x = cam->offset.x, .y = cam->offset.y};
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto get_target(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto cam = pointer_from_value(js, this_val);

    auto obj = JS_NewObjectClass(js, math::vector2::class_id(js));
    auto vec = new Vector2 {.x = cam->target.x, .y = cam->target.y};
    JS_SetOpaque(obj, vec);
    return obj;
}

static auto get_rotation(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto cam = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, cam->rotation);
}

static auto get_zoom(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto cam = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, cam->zoom);
}

static auto set_offset(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto offset = math::vector2::from_value(js, val);
    if (!offset.has_value()) {
        return offset.error();
    }
    auto cam = pointer_from_value(js, this_val);
    cam->offset = *offset;

    return ::JS_UNDEFINED;
}

static auto set_target(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto target = math::vector2::from_value(js, val);
    if (!target.has_value()) {
        return target.error();
    }
    auto cam = pointer_from_value(js, this_val);
    cam->target = *target;

    return ::JS_UNDEFINED;
}

static auto set_rotation(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto rotation = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Camera rotation must be number");
    }
    ::JS_ToFloat64(js, &rotation, val);
    auto cam = pointer_from_value(js, this_val);
    cam->rotation = static_cast<float>(rotation);

    return ::JS_UNDEFINED;
}

static auto set_zoom(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto zoom = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Camera zoom must be number");
    }
    ::JS_ToFloat64(js, &zoom, val);
    auto cam = pointer_from_value(js, this_val);
    cam->zoom = static_cast<float>(zoom);

    return ::JS_UNDEFINED;
}

static auto to_string(::JSContext *js, JSValue this_val, int, JSValue *) -> ::JSValue {
    auto cam = pointer_from_value(js, this_val);
    const auto str = std::format(
        "Camera {{ offset: {{ x: {}, y: {} }}, target: {{ x: {}, y: {} }}, rotation: {}, zoom: {} }}",
        cam->offset.x,
        cam->offset.y,
        cam->target.x,
        cam->target.y,
        cam->rotation,
        cam->zoom
    );
    return JS_NewString(js, str.c_str());
}

static const auto PROTO_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CGETSET_DEF("offset", get_offset, set_offset),
    ::JSCFunctionListEntry JS_CGETSET_DEF("target", get_target, set_target),
    ::JSCFunctionListEntry JS_CGETSET_DEF("rotation", get_rotation, set_rotation),
    ::JSCFunctionListEntry JS_CGETSET_DEF("zoom", get_zoom, set_zoom),
    ::JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

static const auto STATIC_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CFUNC_DEF("default", 0, default_camera),
};

static const auto CAMERA_CLASS = ::JSClassDef {
    .class_name = "Camera",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:Camera", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), class_id(js), &CAMERA_CLASS);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int{PROTO_FUNCS.size()});
        ::JS_SetClassProto(js, class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, constructor, "Camera", 4, ::JS_CFUNC_constructor, 0);
        ::JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), int{STATIC_FUNCS.size()});
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(Camera2D cam) -> std::string {
    return std::format(
        "Camera {{ offset: {{{}, {}}}, target: {{{}, {}}}, rotation: {}, zoom: {} }}",
        cam.offset.x,
        cam.offset.y,
        cam.target.x,
        cam.target.y,
        cam.rotation,
        cam.zoom
    );
}

} // namespace muen::plugins::graphics::camera

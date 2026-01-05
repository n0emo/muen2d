#include <plugins/math.hpp>

#include <array>
#include <format>

#include <raylib.h>

#include <defer.hpp>

namespace muen::plugins::math::rectangle {

auto class_id(JSContext *js) -> ::JSClassID {
    return class_id(JS_GetRuntime(js));
}

auto class_id(JSRuntime *rt) -> ::JSClassID {
    static const auto id = [](auto rt) -> ::JSClassID {
        auto id = ::JSClassID {};
        ::JS_NewClassID(rt, &id);
        return id;
    }(rt);
    return id;
}

auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::Rectangle, ::JSValue> {
    const auto id = class_id(js);
    if (JS_GetClassID(val) == id) {
        const auto ptr = static_cast<::Rectangle *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    auto rec = ::Rectangle {};
    auto prop = ::JSValue {};
    auto temp = double {};

    prop = ::JS_GetPropertyStr(js, val, "x");
    ::JS_ToFloat64(js, &temp, prop);
    rec.x = static_cast<float>(temp);
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "y");
    ::JS_ToFloat64(js, &temp, prop);
    rec.y = static_cast<float>(temp);
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "width");
    ::JS_ToFloat64(js, &temp, prop);
    rec.width = static_cast<float>(temp);
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "height");
    ::JS_ToFloat64(js, &temp, prop);
    rec.height = static_cast<float>(temp);
    ::JS_FreeValue(js, prop);

    if (::JS_HasException(js)) {
        return std::unexpected(::JS_GetException(js));
    } else {
        return rec;
    }
}

auto pointer_from_value(::JSContext *js, ::JSValueConst val) -> ::Rectangle * {
    return static_cast<Rectangle *>(JS_GetOpaque(val, class_id(js)));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 4) {
        return JS_ThrowTypeError(js, "Rectangle constructor expects 4 arguments but %d were provided", argc);
    }

    for (int i = 0; i < argc; i++) {
        if (!::JS_IsNumber(argv[i])) {
            return JS_ThrowTypeError(js, "Rectangle constructor accepts only numbers");
        }
    }

    auto x = double {}, y = double {}, width = double {}, height = double {};
    ::JS_ToFloat64(js, &x, argv[0]);
    ::JS_ToFloat64(js, &y, argv[1]);
    ::JS_ToFloat64(js, &width, argv[2]);
    ::JS_ToFloat64(js, &height, argv[3]);

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

    auto rec = new Rectangle {
        .x = float(x),
        .y = float(y),
        .width = float(width),
        .height = float(height),
    };
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) {
    auto ptr = static_cast<Rectangle *>(JS_GetOpaque(val, class_id(rt)));
    delete ptr;
}

static auto zero(JSContext *js, JSValue this_value, int argc, JSValue *argv) -> JSValue {
    // TODO: maybe we should get proto from this_value?
    auto obj = JS_NewObjectClass(js, class_id(js));
    auto rec = new Rectangle {.x = 0, .y = 0, .width = 0, .height = 0};
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto from_vectors(JSContext *js, JSValue this_value, int argc, JSValue *argv) -> JSValue {
    if (argc != 2) {
        return JS_ThrowTypeError(js, "Rectangle.fromVectors expects 2 arguments, but %d were provided", argc);
    }

    auto pos = vector2::from_value(js, argv[0]);
    if (!pos.has_value()) {
        return pos.error();
    }
    auto size = vector2::from_value(js, argv[1]);
    if (!size.has_value()) {
        return size.error();
    }

    // TODO: maybe we should get proto from this_value?
    auto obj = JS_NewObjectClass(js, class_id(js));
    auto rec = new Rectangle {
        .x = pos->x,
        .y = pos->y,
        .width = size->x,
        .height = size->y,
    };
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto get_x(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto rec = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, rec->x);
}

static auto get_y(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto rec = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, rec->y);
}

static auto get_width(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto rec = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, rec->width);
}

static auto get_height(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto rec = pointer_from_value(js, this_val);
    return ::JS_NewFloat64(js, rec->height);
}

static auto set_x(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto x = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Rectangle x must be number");
    }
    ::JS_ToFloat64(js, &x, val);
    auto rec = pointer_from_value(js, this_val);
    rec->x = float(x);

    return ::JS_UNDEFINED;
}

static auto set_y(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto y = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Rectangle y must be number");
    }
    ::JS_ToFloat64(js, &y, val);
    auto rec = pointer_from_value(js, this_val);
    rec->y = float(y);

    return ::JS_UNDEFINED;
}

static auto set_width(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto width = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Rectangle width must be number");
    }
    ::JS_ToFloat64(js, &width, val);
    auto rec = pointer_from_value(js, this_val);
    rec->width = float(width);

    return ::JS_UNDEFINED;
}

static auto set_height(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto height = double {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "Rectangle height must be number");
    }
    ::JS_ToFloat64(js, &height, val);
    auto rec = pointer_from_value(js, this_val);
    rec->height = float(height);

    return ::JS_UNDEFINED;
}

static auto to_string(::JSContext *js, JSValue this_val, int argc, JSValue *argv) -> ::JSValue {
    auto rec = pointer_from_value(js, this_val);
    const auto str =
        std::format("Rectangle {{ x: {}, y: {}, width: {}, height: {} }}", rec->x, rec->y, rec->width, rec->height);
    return JS_NewString(js, str.c_str());
}

static const auto PROTO_FUNCS = std::array {
    ::JSCFunctionListEntry JS_CGETSET_DEF("x", get_x, set_x),
    ::JSCFunctionListEntry JS_CGETSET_DEF("y", get_y, set_y),
    ::JSCFunctionListEntry JS_CGETSET_DEF("width", get_width, set_width),
    ::JSCFunctionListEntry JS_CGETSET_DEF("height", get_height, set_height),
    ::JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

static const auto STATIC_FUNCS = std::array {
// static const auto STATIC_FUNCS = std::vector<::JSCFunctionListEntry>{
    ::JSCFunctionListEntry JS_CFUNC_DEF("zero", 0, zero),
    ::JSCFunctionListEntry JS_CFUNC_DEF("fromVectors", 0, from_vectors),
};

static const auto RECTANGLE_CLASS = ::JSClassDef {
    .class_name = "Vector2",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:Rectangle", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), class_id(js), &RECTANGLE_CLASS);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), PROTO_FUNCS.size());
        ::JS_SetClassProto(js, class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, constructor, "Rectangle", 1, ::JS_CFUNC_constructor, 0);
        ::JS_SetPropertyFunctionList(js, ctor, STATIC_FUNCS.data(), STATIC_FUNCS.size());
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::math::rectangle

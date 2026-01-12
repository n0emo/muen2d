#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <format>

#include <raylib.h>

#include <defer.hpp>
#include <plugins/math.hpp>

namespace muen::plugins::graphics::npatch {

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

auto from_value(::JSContext *js, ::JSValueConst val) -> std::expected<::NPatchInfo, ::JSValue> {
    const auto id = class_id(js);
    if (JS_GetClassID(val) == id) {
        const auto ptr = static_cast<::NPatchInfo *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    auto npatch = ::NPatchInfo {};
    auto prop = ::JSValue {};

    prop = ::JS_GetPropertyStr(js, val, "source");
    auto source = math::rectangle::from_value(js, prop);
    ::JS_FreeValue(js, prop);
    if (!source.has_value()) {
        return std::unexpected(source.error());
    }
    npatch.source = *source;

    prop = ::JS_GetPropertyStr(js, val, "left");
    auto left = int32_t {};
    ::JS_ToInt32(js, &left, prop);
    npatch.left = left;
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "top");
    auto top = int32_t {};
    ::JS_ToInt32(js, &top, prop);
    npatch.top = top;
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "right");
    auto right = int32_t {};
    ::JS_ToInt32(js, &right, prop);
    npatch.right = right;
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "bottom");
    auto bottom = int32_t {};
    ::JS_ToInt32(js, &bottom, prop);
    npatch.bottom = bottom;
    ::JS_FreeValue(js, prop);

    prop = ::JS_GetPropertyStr(js, val, "layout");
    auto layout = int32_t {};
    ::JS_ToInt32(js, &layout, prop);
    npatch.layout = layout;
    ::JS_FreeValue(js, prop);

    if (::JS_HasException(js)) {
        return std::unexpected(::JS_GetException(js));
    } else {
        return npatch;
    }
}

auto pointer_from_value(::JSContext *js, ::JSValueConst val) -> ::NPatchInfo * {
    return static_cast<NPatchInfo *>(JS_GetOpaque(val, class_id(js)));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    if (argc != 6) {
        return JS_ThrowTypeError(js, "NPatch constructor expects 6 arguments but %d were provided", argc);
    }

    auto source = math::rectangle::from_value(js, argv[0]);
    if (!source.has_value()) {
        return source.error();
    }

    for (int i = 1; i < 6; i++) {
        if (!::JS_IsNumber(argv[i])) {
            return JS_ThrowTypeError(js, "NPatch left, top, right, bottom, and layout must be numbers");
        }
    }

    auto left = int32_t {}, top = int32_t {}, right = int32_t {}, bottom = int32_t {}, layout = int32_t {};
    ::JS_ToInt32(js, &left, argv[1]);
    ::JS_ToInt32(js, &top, argv[2]);
    ::JS_ToInt32(js, &right, argv[3]);
    ::JS_ToInt32(js, &bottom, argv[4]);
    ::JS_ToInt32(js, &layout, argv[5]);

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

    auto npatch = new NPatchInfo {
        .source = *source,
        .left = left,
        .top = top,
        .right = right,
        .bottom = bottom,
        .layout = layout,
    };
    JS_SetOpaque(obj, npatch);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) {
    auto ptr = static_cast<NPatchInfo *>(JS_GetOpaque(val, class_id(rt)));
    delete ptr;
}

static auto get_source(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto npatch = pointer_from_value(js, this_val);
    
    auto obj = JS_NewObjectClass(js, math::rectangle::class_id(js));
    auto rec = new Rectangle {
        .x = npatch->source.x,
        .y = npatch->source.y,
        .width = npatch->source.width,
        .height = npatch->source.height,
    };
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto get_left(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, npatch->left);
}

static auto get_top(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, npatch->top);
}

static auto get_right(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, npatch->right);
}

static auto get_bottom(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, npatch->bottom);
}

static auto get_layout(::JSContext *js, ::JSValueConst this_val) -> ::JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return ::JS_NewInt32(js, npatch->layout);
}

static auto set_source(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto source = math::rectangle::from_value(js, val);
    if (!source.has_value()) {
        return source.error();
    }
    auto npatch = pointer_from_value(js, this_val);
    npatch->source = *source;

    return ::JS_UNDEFINED;
}

static auto set_left(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto left = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "NPatch left must be number");
    }
    ::JS_ToInt32(js, &left, val);
    auto npatch = pointer_from_value(js, this_val);
    npatch->left = left;

    return ::JS_UNDEFINED;
}

static auto set_top(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto top = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "NPatch top must be number");
    }
    ::JS_ToInt32(js, &top, val);
    auto npatch = pointer_from_value(js, this_val);
    npatch->top = top;

    return ::JS_UNDEFINED;
}

static auto set_right(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto right = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "NPatch right must be number");
    }
    ::JS_ToInt32(js, &right, val);
    auto npatch = pointer_from_value(js, this_val);
    npatch->right = right;

    return ::JS_UNDEFINED;
}

static auto set_bottom(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto bottom = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "NPatch bottom must be number");
    }
    ::JS_ToInt32(js, &bottom, val);
    auto npatch = pointer_from_value(js, this_val);
    npatch->bottom = bottom;

    return ::JS_UNDEFINED;
}

static auto set_layout(::JSContext *js, ::JSValueConst this_val, ::JSValueConst val) -> ::JSValue {
    auto layout = int32_t {};
    if (!::JS_IsNumber(val)) {
        return JS_ThrowTypeError(js, "NPatch layout must be number");
    }
    ::JS_ToInt32(js, &layout, val);
    auto npatch = pointer_from_value(js, this_val);
    npatch->layout = layout;

    return ::JS_UNDEFINED;
}

static auto to_string(::JSContext *js, JSValue this_val, int, JSValue *) -> ::JSValue {
    auto npatch = pointer_from_value(js, this_val);
    const auto str = std::format(
        "NPatch {{ source: {{ x: {}, y: {}, width: {}, height: {} }}, left: {}, top: {}, right: {}, bottom: {}, layout: {} }}",
        npatch->source.x, npatch->source.y, npatch->source.width, npatch->source.height,
        npatch->left, npatch->top, npatch->right, npatch->bottom, npatch->layout
    );
    return JS_NewString(js, str.c_str());
}

static const auto PROTO_FUNCS = std::array{
    ::JSCFunctionListEntry JS_CGETSET_DEF("source", get_source, set_source),
    ::JSCFunctionListEntry JS_CGETSET_DEF("left", get_left, set_left),
    ::JSCFunctionListEntry JS_CGETSET_DEF("top", get_top, set_top),
    ::JSCFunctionListEntry JS_CGETSET_DEF("right", get_right, set_right),
    ::JSCFunctionListEntry JS_CGETSET_DEF("bottom", get_bottom, set_bottom),
    ::JSCFunctionListEntry JS_CGETSET_DEF("layout", get_layout, set_layout),
    ::JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

static const auto NPATCH_CLASS = ::JSClassDef {
    .class_name = "NPatch",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(::JSContext *js) -> ::JSModuleDef * {
    auto m = ::JS_NewCModule(js, "muen:NPatch", [](auto js, auto m) -> int {
        ::JS_NewClass(::JS_GetRuntime(js), class_id(js), &NPATCH_CLASS);

        ::JSValue proto = ::JS_NewObject(js);
        ::JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int{PROTO_FUNCS.size()});
        ::JS_SetClassProto(js, class_id(js), proto);

        ::JSValue ctor = ::JS_NewCFunction2(js, constructor, "NPatch", 6, ::JS_CFUNC_constructor, 0);
        ::JS_SetConstructor(js, ctor, proto);

        ::JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    ::JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::math::npatch

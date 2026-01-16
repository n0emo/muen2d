#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <format>

#include <raylib.h>

#include <defer.hpp>
#include <plugins/math.hpp>

namespace js {

using namespace muen;

template<>
auto try_as<NPatchInfo>(JSContext *js, JSValueConst val) -> std::expected<NPatchInfo, JSValue> {
    using namespace muen::plugins;
    const auto id = class_id<&graphics::font::CLASS>(js);
    if (JS_GetClassID(val) == id) {
        const auto ptr = static_cast<NPatchInfo *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    auto np = NPatchInfo {};

    if (auto source = try_get_property<Rectangle>(js, val, "source")) np.source = *source;
    else return Err(source.error());
    if (auto left = try_get_property<int>(js, val, "left")) np.left = *left;
    else return Err(left.error());
    if (auto top = try_get_property<int>(js, val, "top")) np.top = *top;
    else return Err(top.error());
    if (auto right = try_get_property<int>(js, val, "right")) np.right = *right;
    else return Err(right.error());
    if (auto bottom = try_get_property<int>(js, val, "bottom")) np.bottom = *bottom;
    else return Err(bottom.error());
    if (auto layout = try_get_property<int>(js, val, "layout")) np.layout = *layout;
    else return Err(layout.error());

    return np;
}
} // namespace js

namespace muen::plugins::graphics::npatch {

static auto constructor(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValue val) -> void;
static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_left(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_top(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_right(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_bottom(JSContext *js, JSValueConst this_val) -> JSValue;
static auto get_layout(JSContext *js, JSValueConst this_val) -> JSValue;
static auto set_source(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_left(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_top(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_right(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_bottom(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto set_layout(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto to_string(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue;

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("source", get_source, set_source),
    JSCFunctionListEntry JS_CGETSET_DEF("left", get_left, set_left),
    JSCFunctionListEntry JS_CGETSET_DEF("top", get_top, set_top),
    JSCFunctionListEntry JS_CGETSET_DEF("right", get_right, set_right),
    JSCFunctionListEntry JS_CGETSET_DEF("bottom", get_bottom, set_bottom),
    JSCFunctionListEntry JS_CGETSET_DEF("layout", get_layout, set_layout),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

extern const JSClassDef CLASS = {
    .class_name = "NPatch",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:NPatch", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&CLASS>(js), &CLASS);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&CLASS>(js), proto);

        JSValue ctor = JS_NewCFunction2(js, constructor, "NPatch", 6, JS_CFUNC_constructor, 0);
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(NPatchInfo npatch) -> std::string {
    return std::format(
        "NPatch {{ source: {}, left: {}, top: {}, right: {}, bottom: {}, layout: {} }}",
        npatch.source,
        npatch.left,
        npatch.top,
        npatch.right,
        npatch.bottom,
        npatch.layout
    );
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    const auto args = js::unpack_args<Rectangle, int, int, int, int, int>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [source, left, top, right, bottom, layout] = *args;

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&CLASS>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto npatch = new NPatchInfo {
        .source = source,
        .left = left,
        .top = top,
        .right = right,
        .bottom = bottom,
        .layout = layout,
    };
    JS_SetOpaque(obj, npatch);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) -> void {
    auto ptr = static_cast<NPatchInfo *>(JS_GetOpaque(val, js::class_id<&CLASS>(rt)));
    delete ptr;
}

static auto pointer_from_value(JSContext *js, JSValueConst val) -> NPatchInfo * {
    return static_cast<NPatchInfo *>(JS_GetOpaque(val, js::class_id<&CLASS>(js)));
}

static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue {
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

static auto get_left(JSContext *js, JSValueConst this_val) -> JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return JS_NewInt32(js, npatch->left);
}

static auto get_top(JSContext *js, JSValueConst this_val) -> JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return JS_NewInt32(js, npatch->top);
}

static auto get_right(JSContext *js, JSValueConst this_val) -> JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return JS_NewInt32(js, npatch->right);
}

static auto get_bottom(JSContext *js, JSValueConst this_val) -> JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return JS_NewInt32(js, npatch->bottom);
}

static auto get_layout(JSContext *js, JSValueConst this_val) -> JSValue {
    auto npatch = pointer_from_value(js, this_val);
    return JS_NewInt32(js, npatch->layout);
}

static auto set_source(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto source = js::try_as<Rectangle>(js, val);
    if (!source) return JS_Throw(js, source.error());
    auto npatch = pointer_from_value(js, this_val);
    npatch->source = *source;
    return JS_UNDEFINED;
}

static auto set_left(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto left = js::try_as<int>(js, val);
    if (!left) return JS_Throw(js, left.error());
    auto npatch = pointer_from_value(js, this_val);
    npatch->left = *left;
    return JS_UNDEFINED;
}

static auto set_top(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto top = js::try_as<int>(js, val);
    if (!top) return JS_Throw(js, top.error());
    auto npatch = pointer_from_value(js, this_val);
    npatch->top = *top;
    return JS_UNDEFINED;
}

static auto set_right(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto right = js::try_as<int>(js, val);
    if (!right) return JS_Throw(js, right.error());
    auto npatch = pointer_from_value(js, this_val);
    npatch->right = *right;
    return JS_UNDEFINED;
}

static auto set_bottom(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto bottom = js::try_as<int>(js, val);
    if (!bottom) return JS_Throw(js, bottom.error());
    auto npatch = pointer_from_value(js, this_val);
    npatch->bottom = *bottom;
    return JS_UNDEFINED;
}

static auto set_layout(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    const auto layout = js::try_as<int>(js, val);
    if (!layout) return JS_Throw(js, layout.error());
    auto npatch = pointer_from_value(js, this_val);
    npatch->layout = *layout;
    return JS_UNDEFINED;
}

static auto to_string(JSContext *js, JSValue this_val, int, JSValue *) -> JSValue {
    auto npatch = pointer_from_value(js, this_val);
    const auto str = to_string(*npatch);
    return JS_NewStringLen(js, str.c_str(), str.size());
}

} // namespace muen::plugins::graphics::npatch

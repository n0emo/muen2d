#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <gsl/gsl>

#include <raylib.h>

#include <defer.hpp>
#include <plugins/math.hpp>

namespace muen::js {

template<>
auto try_into<NPatchInfo>(const Value& val) noexcept -> JSResult<NPatchInfo> {
    if (auto r = plugins::graphics::npatch::NPatchClassData::from_value(val)) return (*r)->npatch;

    auto np = NPatchInfo {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<Rectangle>("source")) np.source = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<int>("left")) np.left = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<int>("top")) np.top = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<int>("right")) np.right = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<int>("bottom")) np.bottom = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<int>("layout")) np.layout = *v;
    else return Unexpected(v.error());

    return np;
}

} // namespace muen::js

namespace muen::plugins::graphics::npatch {

auto NPatchClassData::from_value(const Value& val) -> JSResult<NPatchClassData *> {
    const auto data = static_cast<NPatchClassData *>(JS_GetOpaque(val.cget(), class_id<&NPATCH>(val.ctx())));
    if (data == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of NPatch"));
    return data;
}

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

extern const JSClassDef NPATCH = {
    .class_name = "NPatch",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:NPatch", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&NPATCH>(js), &NPATCH);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&NPATCH>(js), proto);

        JSValue ctor = JS_NewCFunction2(js, constructor, "NPatch", 6, JS_CFUNC_constructor, 0);
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    const auto args = js::unpack_args<Rectangle, int, int, int, int, int>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [source, left, top, right, bottom, layout] = *args;

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&NPATCH>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto npatch = owner<NPatchInfo *>(new NPatchInfo {
        .source = source,
        .left = left,
        .top = top,
        .right = right,
        .bottom = bottom,
        .layout = layout,
    });
    JS_SetOpaque(obj, npatch);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) -> void {
    auto ptr = owner<NPatchClassData *>(JS_GetOpaque(val, js::class_id<&NPATCH>(rt)));
    delete ptr;
}

static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto npatch = (*data)->npatch;

    auto obj = JS_NewObjectClass(js, js::class_id<&math::rectangle::RECTANGLE>(js));
    auto rec = owner<Rectangle *>(new Rectangle {
        .x = npatch.source.x,
        .y = npatch.source.y,
        .width = npatch.source.width,
        .height = npatch.source.height,
    });
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto get_left(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewInt32(js, (*data)->npatch.left);
}

static auto get_top(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewInt32(js, (*data)->npatch.top);
}

static auto get_right(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewInt32(js, (*data)->npatch.right);
}

static auto get_bottom(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewInt32(js, (*data)->npatch.bottom);
}

static auto get_layout(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewInt32(js, (*data)->npatch.layout);
}

static auto set_source(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto source = js::try_into<Rectangle>(js::borrow(js, val));
    if (!source) return jsthrow(source.error());
    (*data)->npatch.source = *source;
    return JS_UNDEFINED;
}

static auto set_left(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto left = js::try_into<int>(js::borrow(js, val));
    if (!left) return jsthrow(left.error());
    (*data)->npatch.left = *left;
    return JS_UNDEFINED;
}

static auto set_top(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto top = js::try_into<int>(js::borrow(js, val));
    if (!top) return jsthrow(top.error());
    (*data)->npatch.top = *top;
    return JS_UNDEFINED;
}

static auto set_right(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto right = js::try_into<int>(js::borrow(js, val));
    if (!right) return jsthrow(right.error());
    (*data)->npatch.right = *right;
    return JS_UNDEFINED;
}

static auto set_bottom(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto bottom = js::try_into<int>(js::borrow(js, val));
    if (!bottom) return jsthrow(bottom.error());
    (*data)->npatch.bottom = *bottom;
    return JS_UNDEFINED;
}

static auto set_layout(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = NPatchClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto layout = js::try_into<int>(js::borrow(js, val));
    if (!layout) return jsthrow(layout.error());
    (*data)->npatch.layout = *layout;
    return JS_UNDEFINED;
}

static auto to_string(JSContext *js, JSValue this_val, int, JSValue *) -> JSValue {
    auto npatch = js::try_into<NPatchInfo>(js::borrow(js, this_val));
    if (!npatch) return jsthrow(npatch.error());
    const auto str = fmt::format("{}", *npatch);
    return JS_NewStringLen(js, str.c_str(), str.size());
}

} // namespace muen::plugins::graphics::npatch

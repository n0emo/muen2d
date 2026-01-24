#include <plugins/graphics.hpp>

#include <array>
#include <gsl/gsl>

#include <spdlog/spdlog.h>

namespace muen::js {

template<>
auto try_into<const rl::RenderTexture *>(const Value& val) noexcept -> JSResult<const rl::RenderTexture *> {
    auto data = plugins::graphics::render_texture::RenderTextureClassData::from_value(val);
    if (!data) return Unexpected(data.error());
    return &(*data)->texture;
}

} // namespace muen::js

namespace muen::plugins::graphics::render_texture {

auto RenderTextureClassData::from_value(const Value& val) -> JSResult<RenderTextureClassData *> {
    const auto data =
        static_cast<RenderTextureClassData *>(JS_GetOpaque(val.cget(), class_id<&RENDER_TEXTURE>(val.ctx())));
    if (data == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of RenderTexture"));
    return data;
}

static auto constructor(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValueConst val) -> void;
static auto get_width(JSContext *js, JSValueConst this_val) -> JSValue;
static auto set_width(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto get_height(JSContext *js, JSValueConst this_val) -> JSValue;
static auto set_height(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue;
static auto to_string(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue;

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("width", get_width, set_width),
    JSCFunctionListEntry JS_CGETSET_DEF("height", get_height, set_height),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

extern const JSClassDef RENDER_TEXTURE = {
    .class_name = "RenderTexture",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:RenderTexture", [](auto js, auto m) -> auto {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&RENDER_TEXTURE>(js), &RENDER_TEXTURE);

        auto proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&RENDER_TEXTURE>(js), proto);

        auto ctor = JS_NewCFunction2(js, constructor, "RenderTexture", 0, JS_CFUNC_constructor, 0);
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "RenderTexture", JS_DupValue(js, ctor));
        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "RenderTexture");
    JS_AddModuleExport(js, m, "default");

    return m;
}

static auto constructor(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("RenderTexture.constructor/{}", argc);
    auto args = js::unpack_args<int, int>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    auto [width, height] = *args;

    auto proto = JS_GetPropertyStr(js, this_val, "prototype");
    if (JS_IsObject(proto)) {
        return JS_ThrowTypeError(js, "Could not get RenderTexture prototype");
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&RENDER_TEXTURE>(js));
    if (JS_IsException(obj)) {
        return obj;
    }

    auto texture = LoadRenderTexture(width, height);
    if (IsRenderTextureValid(texture)) {
        return JS_ThrowInternalError(js, "Could not create RenderTexture");
    }

    auto ptr = owner<RenderTextureClassData *>(new RenderTexture {texture});
    JS_SetOpaque(obj, ptr);

    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst val) -> void {
    auto texture = owner<RenderTextureClassData *>(JS_GetOpaque(val, js::class_id<&RENDER_TEXTURE>(rt)));
    if (texture == nullptr) {
        SPDLOG_WARN("Could not unload RenderTexture because opaque is null");
        return;
    }
    delete texture;
}

static auto get_width(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = RenderTextureClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewNumber(js, double((*data)->texture.texture.width));
}

static auto set_width(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = RenderTextureClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    auto width = js::try_into<int>(js::borrow(js, val));
    if (!width) return jsthrow(width.error());
    auto height = (*data)->texture.texture.height;
    (*data)->texture = rl::RenderTexture::load(*width, height);
    return JS_UNDEFINED;
}

static auto get_height(JSContext *js, JSValueConst this_val) -> JSValue {
    auto data = RenderTextureClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    return JS_NewNumber(js, double((*data)->texture.texture.height));
}

static auto set_height(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto data = RenderTextureClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    auto height = js::try_into<int>(js::borrow(js, val));
    if (!height) return jsthrow(height.error());
    auto width = (*data)->texture.texture.width;
    (*data)->texture = rl::RenderTexture::load(width, *height);
    return JS_UNDEFINED;
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto data = RenderTextureClassData::from_value(borrow(js, this_val));
    if (!data) return jsthrow(data.error());
    const auto str = fmt::format("{}", (*data)->texture);
    return JS_NewStringLen(js, str.data(), str.size());
}

} // namespace muen::plugins::graphics::render_texture

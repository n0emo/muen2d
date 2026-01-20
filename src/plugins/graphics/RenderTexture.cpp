#include <plugins/graphics.hpp>

#include <array>
#include <gsl/gsl>

#include <spdlog/spdlog.h>

namespace muen::js {

using namespace muen;

template<>
auto try_into<RenderTexture *>(const Value& val) noexcept -> JSResult<RenderTexture *> {
    using plugins::graphics::render_texture::RENDER_TEXTURE;

    auto ptr = static_cast<RenderTexture *>(JS_GetOpaque(val.cget(), class_id<&RENDER_TEXTURE>(val.ctx())));
    if (ptr == nullptr) JSError::type_error(val.ctx(), "Not a RenderTexture instance");
    return ptr;
}

template<>
auto try_into<RenderTexture>(const Value& val) noexcept -> JSResult<RenderTexture> {
    auto ptr = try_into<RenderTexture *>(val);
    if (ptr) return **ptr;
    else return Unexpected(ptr.error());
}

} // namespace muen::js

namespace muen::plugins::graphics::render_texture {

using namespace gsl;

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

auto to_string(RenderTexture texture) -> std::string {
    return fmt::format("RenderTexture{{ texture: {}, depth: {}, id: {} }}", texture.texture, texture.depth, texture.id);
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

    auto ptr = owner<RenderTexture *>(new RenderTexture {texture});
    JS_SetOpaque(obj, ptr);

    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst val) -> void {
    auto texture = owner<RenderTexture *>(JS_GetOpaque(val, js::class_id<&RENDER_TEXTURE>(rt)));
    if (texture == nullptr) {
        SPDLOG_WARN("Could not unload RenderTexture because opaque is null");
        return;
    }
    UnloadRenderTexture(*texture);
    delete texture;
}

static auto get_width(JSContext *js, JSValueConst this_val) -> JSValue {
    auto texture = js::try_into<RenderTexture *>(js::borrow(js, this_val));
    if (!texture) return jsthrow(texture.error());
    return JS_NewNumber(js, double((*texture)->texture.width));
}

static auto set_width(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto texture = js::try_into<RenderTexture *>(js::borrow(js, this_val));
    if (!texture) return jsthrow(texture.error());
    auto width = js::try_into<float>(js::borrow(js, val));
    if (!width) return jsthrow(width.error());
    return JS_ThrowPlainError(js, "RenderTexture.width.set is not yet implemented");
}

static auto get_height(JSContext *js, JSValueConst this_val) -> JSValue {
    auto texture = js::try_into<RenderTexture *>(js::borrow(js, this_val));
    if (!texture) return jsthrow(texture.error());
    auto height = (*texture)->texture.height;
    return JS_NewNumber(js, double(height));
}

static auto set_height(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    auto texture = js::try_into<RenderTexture *>(js::borrow(js, this_val));
    if (!texture) return jsthrow(texture.error());
    auto height = js::try_into<float>(js::borrow(js, val));
    if (!height) return jsthrow(height.error());
    return JS_ThrowPlainError(js, "RenderTexture.height.set is not yet implemented");
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto texture = js::try_into<RenderTexture *>(js::borrow(js, this_val));
    if (!texture) return jsthrow(texture.error());
    const auto str = to_string(**texture);
    return JS_NewStringLen(js, str.data(), str.size());
}

} // namespace muen::plugins::graphics::render_texture

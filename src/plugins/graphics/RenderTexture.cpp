#include <plugins/graphics.hpp>

#include <array>

#include <spdlog/spdlog.h>

namespace js {

using namespace muen;

template<>
auto try_as<RenderTexture>(JSContext *js, JSValueConst value) -> std::expected<RenderTexture, JSValue> {
    auto ptr = static_cast<RenderTexture *>(
        JS_GetOpaque(value, js::class_id<&muen::plugins::graphics::render_texture::CLASS>(js))
    );

    if (ptr == nullptr) {
        return Err(JS_NewTypeError(js, "Value is not a RenderTexture instance"));
    }

    return *ptr;
}

} // namespace js

namespace muen::plugins::graphics::render_texture {

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

extern const JSClassDef CLASS = {
    .class_name = "RenderTexture",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:RenderTexture", [](auto js, auto m) -> auto {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&CLASS>(js), &CLASS);

        auto proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&CLASS>(js), proto);

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
    if (!args) {
        return JS_Throw(js, args.error());
    }
    auto [width, height] = *args;

    auto proto = JS_GetPropertyStr(js, this_val, "prototype");
    if (JS_IsObject(proto)) {
        return JS_ThrowTypeError(js, "Could not get RenderTexture prototype");
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&CLASS>(js));
    if (JS_IsException(obj)) {
        return obj;
    }

    auto texture = LoadRenderTexture(width, height);
    if (IsRenderTextureValid(texture)) {
        return JS_ThrowInternalError(js, "Could not create RenderTexture");
    }

    auto ptr = new RenderTexture {texture};
    JS_SetOpaque(obj, ptr);

    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst val) -> void {
    auto texture = static_cast<RenderTexture *>(JS_GetOpaque(val, js::class_id<&CLASS>(rt)));
    if (texture == nullptr) {
        SPDLOG_WARN("Could not unload RenderTexture because opaque is null");
        return;
    }
    UnloadRenderTexture(*texture);
    delete texture;
}

static auto get_width(JSContext *js, JSValueConst this_val) -> JSValue {
    auto texture = js::try_as<RenderTexture>(js, this_val);
    if (!texture) {
        return JS_Throw(js, texture.error());
    }
    auto width = texture->texture.width;
    return JS_NewNumber(js, double(width));
}

static auto set_width(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    (void)this_val;
    (void)val;
    return JS_ThrowPlainError(js, "RenderTexture.width.set is not yet implemented");
}

static auto get_height(JSContext *js, JSValueConst this_val) -> JSValue {
    auto texture = js::try_as<RenderTexture>(js, this_val);
    if (!texture) return JS_Throw(js, texture.error());
    auto height = texture->texture.height;
    return JS_NewNumber(js, double(height));
}

static auto set_height(JSContext *js, JSValueConst this_val, JSValueConst val) -> JSValue {
    (void)this_val;
    (void)val;
    return JS_ThrowPlainError(js, "RenderTexture.height.set is not yet implemented");
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    auto texture = js::try_as<RenderTexture>(js, this_val);
    if (!texture) return JS_Throw(js, texture.error());
    const auto str = to_string(*texture);
    return JS_NewStringLen(js, str.data(), str.size());
}

} // namespace muen::plugins::graphics::render_texture

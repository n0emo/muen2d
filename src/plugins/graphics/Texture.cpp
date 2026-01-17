#include <plugins/graphics.hpp>

#include <array>
#include <expected>

#include <fmt/format.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <engine.hpp>
#include <plugins/math.hpp>

namespace js {

using muen::Err;

template<>
auto try_as<Texture>(JSContext *js, JSValueConst val) -> std::expected<Texture, JSValue> {
    const auto id = class_id<&muen::plugins::graphics::texture::CLASS>(js);
    if (JS_GetClassID(val) == id) {
        const auto ptr = static_cast<::Texture *>(JS_GetOpaque(val, id));
        return *ptr;
    }

    return Err(JS_ThrowTypeError(js, "Expected Texture object"));
}

} // namespace js

namespace muen::plugins::graphics::texture {

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValue val) -> void;
static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue;
static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue;

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("source", get_source, nullptr),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

extern const JSClassDef CLASS = {
    .class_name = "Texture",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Texture", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&CLASS>(js), &CLASS);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&CLASS>(js), proto);

        JSValue ctor = JS_NewCFunction2(js, constructor, "Texture", 1, JS_CFUNC_constructor, 0);
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "default", ctor);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

auto to_string(Texture tex) -> std::string {
    return fmt::format(
        "Texture {{ id: {}, width: {}, height: {}, mipmaps: {}, format: {} }}",
        tex.id,
        tex.width,
        tex.height,
        tex.mipmaps,
        tex.format
    );
}

auto pointer_from_value(JSContext *js, JSValueConst val) -> Texture * {
    return static_cast<Texture *>(JS_GetOpaque(val, js::class_id<&CLASS>(js)));
}

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    SPDLOG_TRACE("Texture.constructor/{}", argc);
    const auto args = js::unpack_args<std::string>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [filename] = *args;

    auto e = engine::from_js(js);
    const auto path = engine::resolve_path(e, filename);

    SPDLOG_TRACE("LoadTexture({})", path);
    auto texture = LoadTexture(path.c_str());

    if (texture.id == 0) {
        return JS_ThrowInternalError(js, "Could not load texture: %s", filename.c_str());
    }

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        UnloadTexture(texture);
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&CLASS>(js));
    if (JS_HasException(js)) {
        UnloadTexture(texture);
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto tex = new Texture(texture);
    JS_SetOpaque(obj, tex);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) -> void {
    auto ptr = static_cast<Texture *>(JS_GetOpaque(val, js::class_id<&CLASS>(rt)));
    if (ptr) {
        UnloadTexture(*ptr);
        delete ptr;
    }
}

static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto tex = pointer_from_value(js, this_val);
    const auto obj = JS_NewObjectClass(js, math::rectangle::class_id(js));
    const auto rec = new Rectangle {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(tex->width),
        .height = static_cast<float>(tex->height),
    };
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    const auto tex = pointer_from_value(js, this_val);
    const auto str = to_string(*tex);
    return JS_NewString(js, str.c_str());
}

} // namespace muen::plugins::graphics::texture

#include <plugins/graphics.hpp>

#include <array>
#include <expected>

#include <fmt/format.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <engine.hpp>
#include <plugins/math.hpp>

namespace muen::js {

template<>
auto try_into<Texture *>(const Value& val) noexcept -> JSResult<Texture *> {
    using muen::plugins::graphics::texture::TEXTURE;

    const auto ptr = static_cast<Texture *>(JS_GetOpaque(val.cget(), class_id<&TEXTURE>(val.ctx())));
    if (ptr == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Expected Texture object"));
    return ptr;
}

template<>
auto try_into<Texture>(const Value& val) noexcept -> JSResult<Texture> {
    auto ptr = try_into<Texture *>(val);
    if (ptr) return **ptr;
    else return Unexpected(ptr.error());
}

} // namespace muen::js

namespace muen::plugins::graphics::texture {

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValue val) -> void;
static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue;
static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue;

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("source", get_source, nullptr),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

extern const JSClassDef TEXTURE = {
    .class_name = "Texture",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Texture", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&TEXTURE>(js), &TEXTURE);

        JSValue proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&TEXTURE>(js), proto);

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

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    SPDLOG_TRACE("Texture.constructor/{}", argc);
    const auto args = js::unpack_args<std::string>(js, argc, argv);
    if (!args) return jsthrow(args.error());
    const auto [filename] = *args;
    const auto path = std::filesystem::path(filename);

    auto& e = Engine::get(js);
    auto data = e.store().read_bytes(path);
    if (!data) {
        return JS_ThrowInternalError(
            js,
            "%s",
            std::format("Could not load texture `{}`: {}", filename, data.error()->msg()).c_str()
        );
    }

    auto image = LoadImageFromMemory(
        path.extension().string().c_str(),
        // NOLINTNEXTLINE: Casting from char* to unsigned char* is explicitly allowed by the standard
        reinterpret_cast<unsigned char *>(data->data()),
        int(data->size())
    );

    auto texture = LoadTextureFromImage(image);
    UnloadImage(image);

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        UnloadTexture(texture);
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&TEXTURE>(js));
    if (JS_HasException(js)) {
        UnloadTexture(texture);
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto tex = owner<Texture *> {new (std::nothrow) Texture(texture)};
    JS_SetOpaque(obj, tex);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) -> void {
    auto ptr = owner<Texture *> {static_cast<Texture *>(JS_GetOpaque(val, js::class_id<&TEXTURE>(rt)))};

    if (!ptr) return;
    UnloadTexture(*ptr);
    delete ptr;
}

static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto tex = js::try_into<Texture *>(js::borrow(js, this_val));
    if (!tex) return jsthrow(tex.error());

    const auto obj = JS_NewObjectClass(js, js::class_id<&math::rectangle::RECTANGLE>(js));
    const auto rec = owner<Rectangle *> {new (std::nothrow) Rectangle {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>((*tex)->width),
        .height = static_cast<float>((*tex)->height),
    }};
    JS_SetOpaque(obj, rec);
    return obj;
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    const auto tex = js::try_into<Texture *>(js::borrow(js, this_val));
    if (!tex) return jsthrow(tex.error());
    const auto str = to_string(**tex);
    return JS_NewString(js, str.c_str());
}

} // namespace muen::plugins::graphics::texture

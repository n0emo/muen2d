#include <plugins/graphics.hpp>

#include <array>
#include <expected>
#include <gsl/gsl>

#include <fmt/format.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <engine.hpp>
#include <error.hpp>
#include <plugins/math.hpp>

namespace muen::plugins::graphics::texture {} // namespace muen::plugins::graphics::texture

namespace muen::js {

using muen::plugins::graphics::texture::TEXTURE;
using muen::plugins::graphics::texture::TextureClassData;
using plugins::graphics::texture::TextureOptions;

template<>
auto try_into<const rl::Texture *>(const Value& val) noexcept -> JSResult<const rl::Texture *> {
    const auto data = static_cast<TextureClassData *>(JS_GetOpaque(val.cget(), class_id<&TEXTURE>(val.ctx())));
    if (data == nullptr) return Unexpected(JSError::type_error(val.ctx(), "Expected Texture object"));

    auto& e = Engine::get(val.ctx());
    auto& tex = e.texture_store().borrow(data->handle);
    return &tex;
}

template<>
auto try_into<TextureOptions>(const Value& val) noexcept -> JSResult<TextureOptions> try {
    auto o = TextureOptions {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<std::optional<std::string>>("path"); v) o.path = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<std::optional<std::string>>("name"); v) o.name = *v;
    else return Unexpected(v.error());

    return o;
} catch (std::exception& e) {
    return Unexpected(JSError::plain_error(val.ctx(), fmt::format("Unexpected C++ exception: {}", e.what())));
}

} // namespace muen::js

namespace muen::plugins::graphics::texture {

using namespace gsl;

inline auto read_texture_options_from_args(not_null<JSContext *> ctx, int argc, JSValueConst *argv) noexcept
    -> js::JSResult<TextureLoadMode> try {
    if (auto args_string = js::unpack_args<std::string>(ctx, argc, argv)) {
        auto [path] = std::move(*args_string);
        return TextureLoadByParams {
            .path = path,
            .name = path,
        };
    } else if (auto args_options = js::unpack_args<TextureOptions>(ctx, argc, argv)) {
        auto [opts] = std::move(*args_options);
        if (opts.name && !opts.path) {
            return TextureLoadByName {
                .name = std::move(*opts.name),
            };
        } else if (opts.path) {
            if (!opts.name) opts.name = opts.path->string();
            return TextureLoadByParams {
                .path = std::move(*opts.path),
                .name = std::move(*opts.name),
            };
        } else {
            return Unexpected(js::JSError::type_error(ctx, "Either name or path must be present in options"));
        }
    } else {
        return Unexpected(js::JSError::type_error(ctx, "No matching overload"));
    }
} catch (std::exception& e) {
    return Unexpected(JSError::plain_error(ctx, fmt::format("Unexpected C++ exception: {}", e.what())));
}

static auto constructor(JSContext *js, JSValueConst new_target, int argc, JSValueConst *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValueConst val) -> void;
static auto unload(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue;
static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue;
static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue;

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("source", get_source, nullptr),
    JSCFunctionListEntry JS_CFUNC_DEF("unload", 0, unload),
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

static auto constructor(JSContext *js, JSValue new_target, int argc, JSValue *argv) -> JSValue {
    SPDLOG_TRACE("Texture.constructor/{}", argc);
    auto opts = read_texture_options_from_args(js, argc, argv);
    if (!opts) return jsthrow(opts.error());
    auto& e = Engine::get(js);

    const auto handle = std::visit(
        [&](auto&& arg) -> js::JSResult<int> {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, TextureLoadByName>) {
                return e.texture_store().load_by_name(arg.name);
            } else if constexpr (std::is_same_v<T, TextureLoadByParams>) {
                return e.texture_store().load(arg.name, [&]() -> TextureData {
                    return TextureData::load(arg.path, e.file_store());
                });
            } else if constexpr (std::is_same_v<T, std::monostate>) {
                return Unexpected(js::JSError::type_error(js, "Either name or path must be present in options"));
            } else {
                static_assert(false, "Non-exhaustive visit");
            }
        },
        std::move(*opts)
    );

    auto proto = JS_GetPropertyStr(js, new_target, "prototype");
    if (JS_IsException(proto)) {
        return proto;
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&TEXTURE>(js));
    if (JS_HasException(js)) {
        JS_FreeValue(js, obj);
        return JS_GetException(js);
    }

    auto data = owner<TextureClassData *> {new (std::nothrow) TextureClassData {.handle = *handle}};
    JS_SetOpaque(obj, data);
    return obj;
}

static auto finalizer(JSRuntime *rt, JSValue val) -> void {
    SPDLOG_TRACE("Finalizing Texture");
    auto ptr = owner<TextureClassData *>(JS_GetOpaque(val, js::class_id<&TEXTURE>(rt)));
    if (!ptr) {
        SPDLOG_WARN("Could not unload Texture because opaque is null");
        return;
    }
    auto& e = Engine::get(rt);
    e.texture_store().release(ptr->handle);
    delete ptr;
}

static auto unload(JSContext *js, JSValueConst this_val, int argc, JSValueConst *) -> JSValue {
    SPDLOG_TRACE("Texture.unload/{}", argc);
    auto ptr = owner<TextureClassData *>(JS_GetOpaque(this_val, js::class_id<&TEXTURE>(js)));
    if (!ptr) return JS_ThrowTypeError(js, "Not an instance of Texture");
    auto& e = Engine::get(js);
    e.texture_store().release(ptr->handle);
    return JS_UNDEFINED;
}

static auto get_source(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto tex = js::try_into<const rl::Texture *>(js::borrow(js, this_val));
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
    const auto tex = js::try_into<const rl::Texture *>(js::borrow(js, this_val));
    if (!tex) return jsthrow(tex.error());
    const auto str = fmt::format("{}", **tex);
    return JS_NewString(js, str.c_str());
}

} // namespace muen::plugins::graphics::texture

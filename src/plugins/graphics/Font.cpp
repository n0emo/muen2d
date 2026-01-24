#include <plugins/graphics.hpp>

#include <array>

#include <engine.hpp>
#include <defer.hpp>

namespace muen::js {

using plugins::graphics::font::CLASS;
using plugins::graphics::font::FontClassData;
using plugins::graphics::font::FontOptions;

template<>
auto try_into<const rl::Font *>(const Value& val) noexcept -> JSResult<const rl::Font *> {
    const auto data = static_cast<FontClassData *>(JS_GetOpaque(val.cget(), class_id<&CLASS>(val.ctx())));
    if (!data) return Unexpected(JSError::type_error(val.ctx(), "Not an instance of a Font"));

    auto& e = Engine::get(val.ctx());
    auto& font = e.font_store().borrow(data->handle);
    return &font;
}

template<>
auto try_into<FontOptions>(const Value& val) noexcept -> JSResult<FontOptions> try {
    auto o = FontOptions {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<std::optional<std::string>>("path")) o.path = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<std::optional<std::string>>("name")) o.name = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<std::optional<int>>("fontSize")) o.font_size = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<std::optional<std::vector<int>>>("codepoints")) o.codepoints = *v;
    else return Unexpected(v.error());

    return o;
} catch (std::exception& e) {
    return Unexpected(JSError::plain_error(val.ctx(), fmt::format("Unexpected C++ exception: {}", e.what())));
}

} // namespace muen::js

namespace muen::plugins::graphics::font {

using namespace gsl;

inline auto read_font_options_from_args(not_null<JSContext *> ctx, int argc, JSValueConst *argv) noexcept
    -> js::JSResult<FontLoadMode> try {
    if (auto args_string = js::unpack_args<std::string>(ctx, argc, argv)) {
        auto [path] = std::move(*args_string);
        return FontLoadByParams {
            .path = path,
            .name = path,
            .font_size = 32,
            .codepoints = std::nullopt,
        };
    } else if (auto args_options = js::unpack_args<FontOptions>(ctx, argc, argv)) {
        auto [opts] = std::move(*args_options);
        if (opts.name && !opts.path && !opts.font_size && !opts.codepoints) {
            return FontLoadByName {
                .name = std::move(*opts.name),
            };
        } else if (opts.path) {
            if (!opts.name) opts.name = opts.path->string();
            return FontLoadByParams {
                .path = std::move(*opts.path),
                .name = std::move(*opts.name),
                .font_size = opts.font_size.value_or(32),
                .codepoints = std::move(opts.codepoints),
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

static auto constructor(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue;
static auto finalizer(JSRuntime *rt, JSValueConst this_val) -> void;
static auto get_valid(::JSContext *js, ::JSValueConst this_val) -> ::JSValue;
static auto to_string(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue;

extern const JSClassDef CLASS = {
    .class_name = "Font",
    .finalizer = finalizer,
    .gc_mark = nullptr,
    .call = nullptr,
    .exotic = nullptr,
};

static const auto PROTO_FUNCS = std::array {
    JSCFunctionListEntry JS_CGETSET_DEF("valid", get_valid, nullptr),
    JSCFunctionListEntry JS_CFUNC_DEF("toString", 0, to_string),
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:Font", [](auto js, auto m) -> int {
        JS_NewClass(JS_GetRuntime(js), js::class_id<&CLASS>(js), &CLASS);

        auto proto = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, proto, PROTO_FUNCS.data(), int {PROTO_FUNCS.size()});
        JS_SetClassProto(js, js::class_id<&CLASS>(js), proto);

        auto ctor = JS_NewCFunction2(js, constructor, "Font", 0, ::JS_CFUNC_constructor, 0);
        JS_SetConstructor(js, ctor, proto);

        JS_SetModuleExport(js, m, "Font", JS_DupValue(js, ctor));
        JS_SetModuleExport(js, m, "default", ctor);
        return 0;
    });

    JS_AddModuleExport(js, m, "Font");
    JS_AddModuleExport(js, m, "default");

    return m;
}

static auto constructor(JSContext *js, JSValue this_val, int argc, JSValue *argv) -> JSValue {
    auto opts = read_font_options_from_args(js, argc, argv);
    if (!opts) return jsthrow(opts.error());
    auto& e = Engine::get(js);

    const auto handle = std::visit(
        [&](auto&& arg) -> js::JSResult<int> {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, FontLoadByName>) {
                return e.font_store().load_by_name(arg.name);
            } else if constexpr (std::is_same_v<T, FontLoadByParams>) {
                return e.font_store().load(arg.name, [&]() -> FontData {
                    std::optional<std::span<int>> codepoints {};
                    if (arg.codepoints) codepoints = std::span(*arg.codepoints);
                    return FontData::load(arg.path, arg.font_size, codepoints, e.file_store());
                });
            } else if constexpr (std::is_same_v<T, std::monostate>) {
                return Unexpected(js::JSError::type_error(js, "Either name or path must be present in options"));
            } else {
                static_assert(false, "Non-exhaustive visit");
            }
        },
        std::move(*opts)
    );
    if (!handle) return jsthrow(handle.error());

    auto proto = JS_GetPropertyStr(js, this_val, "prototype");
    if (!JS_IsObject(proto)) {
        return JS_ThrowInternalError(js, "Font constructor could not get prototype");
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&CLASS>(js));
    if (!JS_IsObject(obj)) {
        return JS_ThrowInternalError(js, "Font constructor could not create object");
    }

    auto data = owner<FontClassData *>(new FontClassData {.handle = *handle});
    JS_SetOpaque(obj, data);

    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst this_val) -> void {
    auto ptr = owner<FontClassData *>(JS_GetOpaque(this_val, js::class_id<&CLASS>(rt)));
    if (!ptr) {
        SPDLOG_WARN("Could not unload Texture because opaque is null");
        return;
    }
    auto& e = Engine::get(rt);
    e.font_store().release(ptr->handle);
    delete ptr;
}

static auto get_valid(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto font = js::try_into<const rl::Font *>(js::borrow(js, this_val));
    return JS_NewBool(js, IsFontValid(**font));
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    const auto font = js::try_into<const rl::Font *>(js::borrow(js, this_val));
    if (!font) return jsthrow(font.error());
    const auto str = fmt::format("{}", **font);
    return JS_NewStringLen(js, str.data(), str.size());
}

} // namespace muen::plugins::graphics::font

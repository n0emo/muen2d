#include <plugins/graphics.hpp>

#include <array>

#include <engine.hpp>
#include <defer.hpp>

namespace js {

using muen::Err;

template<>
auto try_as<Font>(JSContext *js, JSValueConst value) -> std::expected<Font, JSValue> {
    const auto id = class_id<&muen::plugins::graphics::font::CLASS>(js);
    if (JS_IsObject(value) && id == JS_GetClassID(value)) {
        return *static_cast<Font *>(JS_GetOpaque(value, id));
    } else {
        return Err(JS_NewTypeError(js, "Font must be object of class Font"));
    }
}

} // namespace js

namespace muen::plugins::graphics::font {

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

auto to_string(Font font) -> std::string {
    return fmt::format("Font {{ id: {}, size: {}, glyphCount: {} }}", font.texture.id, font.baseSize, font.glyphCount);
}

auto from_value_unsafe(JSContext *js, ::JSValueConst val) -> Font * {
    return static_cast<Font *>(JS_GetOpaque(val, js::class_id<&CLASS>(js)));
}

static auto constructor(JSContext *js, JSValue this_val, int argc, JSValue *argv) -> JSValue {
    if (argc == 0) {
        return JS_ThrowTypeError(js, "Font constructor expects at least 1 argument");
    }

    auto e = engine::from_js(js);

    const char *path_cstr = JS_ToCString(js, argv[0]);
    if (path_cstr == nullptr) {
        return JS_ThrowTypeError(js, "Font constructor expects string as the first argument");
    }
    const auto path = engine::resolve_path(e, path_cstr);
    JS_FreeCString(js, path_cstr);

    double font_size = 32;
    if (argc > 1) {
        if (!JS_IsNumber(argv[1])) {
            return JS_ThrowTypeError(js, "Font constructor second argument must be number");
        }
        JS_ToFloat64(js, &font_size, argv[1]);
    }

    [[maybe_unused]] auto codepoints = nullptr;
    if (argc > 2) {
        return JS_ThrowPlainError(js, "Font constructor with 3 arguments is not implemented yet");
    }

    auto font = LoadFontEx(path.c_str(), int(font_size), nullptr, 0x1000);
    auto font_ptr = new Font {font};

    auto proto = JS_GetPropertyStr(js, this_val, "prototype");
    if (!JS_IsObject(proto)) {
        return JS_ThrowInternalError(js, "Font constructor could not get prototype");
    }
    defer(JS_FreeValue(js, proto));

    auto obj = JS_NewObjectProtoClass(js, proto, js::class_id<&CLASS>(js));
    if (!JS_IsObject(obj)) {
        return JS_ThrowInternalError(js, "Font constructor could not create object");
    }

    JS_SetOpaque(obj, font_ptr);

    return obj;
}

static auto finalizer(JSRuntime *rt, JSValueConst this_val) -> void {
    auto ptr = static_cast<Font *>(JS_GetOpaque(this_val, js::class_id<&CLASS>(rt)));
    UnloadFont(*ptr);
    delete ptr;
}

static auto get_valid(JSContext *js, JSValueConst this_val) -> JSValue {
    const auto font = from_value_unsafe(js, this_val);
    return JS_NewBool(js, IsFontValid(*font));
}

static auto to_string(JSContext *js, JSValueConst this_val, int, JSValueConst *) -> JSValue {
    const auto font = js::try_as<Font>(js, this_val);
    const auto str = to_string(*font);
    return JS_NewStringLen(js, str.data(), str.size());
}

} // namespace muen::plugins::graphics::font

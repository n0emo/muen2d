#include <plugins/graphics.hpp>

#include <array>
#include <variant>
#include <optional>

#include <raylib.h>
#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <plugins/math.hpp>
#include <jsutil.hpp>

namespace muen::plugins::graphics {

struct Text {
    std::variant<std::string, int, std::vector<int>> text = {};
    float font_size = {};
    Vector2 position = {};
    Color color = {};
    std::optional<Vector2> origin = std::nullopt;
    std::optional<float> rotation = std::nullopt;
    std::optional<float> spacing = std::nullopt;
    std::optional<Font> font = std::nullopt;
};

static auto text_from_value(JSContext *js, JSValueConst value) -> std::expected<Text, JSValue> {
    auto text = Text {};

    if (auto t = js::try_get_property<std::string>(js, value, "text")) {
        text.text = *t;
    } else if (auto c = js::try_get_property<int>(js, value, "codepoint")) {
        text.text = *c;
    } else if (auto cs = js::try_get_property<std::vector<int>>(js, value, "codepoints")) {
        text.text = *cs;
    } else {
        return Err(JS_NewTypeError(
            js,
            "Text object must either have `text` (string), `codepoint` (number) or `codepoints` (number[]) property"
        ));
    }

    if (auto v = js::try_get_property<float>(js, value, "fontSize")) text.font_size = *v;
    else return Err(v.error());
    if (auto v = js::try_get_property<Vector2>(js, value, "position")) text.position = *v;
    else return Err(v.error());
    if (auto v = js::try_get_property<Color>(js, value, "color")) text.color = *v;
    else return Err(v.error());
    if (auto v = js::try_get_property<std::optional<Vector2>>(js, value, "origin")) text.origin = *v;
    else return Err(v.error());
    if (auto v = js::try_get_property<std::optional<float>>(js, value, "rotation")) text.rotation = *v;
    else return Err(v.error());
    if (auto v = js::try_get_property<std::optional<float>>(js, value, "spacing")) text.spacing = *v;
    else return Err(v.error());
    if (auto v = js::try_get_property<std::optional<Font>>(js, value, "font")) text.font = *v;
    else return Err(v.error());

    return text;
}

static auto clear(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.clear/{}", argc);
    const auto args = js::unpack_args<Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [color] = *args;
    SPDLOG_TRACE("ClearBackground({})", color);
    ClearBackground(color);
    return JS_DupValue(js, this_val);
}

static auto circle_simple(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.circle/{}", argc);
    const auto args = js::unpack_args<int, int, float, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [x, y, radius, color] = *args;
    SPDLOG_TRACE("DrawCircle({}, {}, {}, {})", x, y, radius, color);
    DrawCircle(x, y, radius, color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_simple(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.rectangle/{}", argc);
    const auto args = js::unpack_args<int, int, int, int, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [x, y, width, height, color] = *args;
    SPDLOG_TRACE("DrawRectangle({}, {}, {}, {}, {})", x, y, width, height, color);
    DrawRectangle(x, y, width, height, color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_v(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.rectangleV/{}", argc);
    const auto args = js::unpack_args<Vector2, Vector2, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [position, size, color] = *args;
    SPDLOG_TRACE("DrawRectangleV({}, {}, {})", position, size, color);
    DrawRectangleV(position, size, color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_rec(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.rectangleRec/{}", argc);
    const auto args = js::unpack_args<Rectangle, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [rec, color] = *args;
    SPDLOG_TRACE("DrawRectangleRec({}, {})", rec, color);
    DrawRectangleRec(rec, color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_pro(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.rectanglePro/{}", argc);
    const auto args = js::unpack_args<Rectangle, Vector2, float, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [rec, origin, rotation, color] = *args;
    SPDLOG_TRACE("DrawRectanglePro({}, {}, {}, {})", rec, origin, rotation, color);
    DrawRectanglePro(rec, origin, rotation, color);
    return JS_DupValue(js, this_val);
}

static auto begin_camera_mode(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.beginCameraMode/{}", argc);
    const auto args = js::unpack_args<Camera2D>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [camera] = *args;
    SPDLOG_TRACE("BeginMode2D({})", camera);
    BeginMode2D(camera);
    return JS_DupValue(js, this_val);
}

static auto end_camera_mode(JSContext *js, JSValueConst this_val, int argc, JSValueConst *) -> JSValue {
    SPDLOG_TRACE("graphics.beginCameraMode/{}", argc);
    SPDLOG_TRACE("BeginMode2D()");
    EndMode2D();
    return JS_DupValue(js, this_val);
}

static auto texture_simple(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.texture/{}", argc);
    const auto args = js::unpack_args<Texture, int, int, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture, x, y, tint] = *args;
    SPDLOG_TRACE("DrawTexture({}, {}, {}, {})", texture, x, y, tint);
    DrawTexture(texture, x, y, tint);
    return JS_DupValue(js, this_val);
}

static auto texture_v(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.textureV/{}", argc);
    const auto args = js::unpack_args<Texture, Vector2, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture, position, tint] = *args;
    SPDLOG_TRACE("DrawTextureV({}, {}, {})", texture, position, tint);
    DrawTextureV(texture, position, tint);
    return JS_DupValue(js, this_val);
}

static auto texture_ex(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.textureEx/{}", argc);
    const auto args = js::unpack_args<Texture, Vector2, float, float, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture, position, rotation, scale, tint] = *args;
    SPDLOG_TRACE("DrawTextureEx({}, {}, {}, {}, {})", texture, position, rotation, scale, tint);
    DrawTextureEx(texture, position, rotation, scale, tint);
    return JS_DupValue(js, this_val);
}

static auto texture_rec(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.textureRec/{}", argc);
    const auto args = js::unpack_args<Texture, Rectangle, Vector2, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture, source, position, tint] = *args;
    SPDLOG_TRACE("DrawTextureRec({}, {}, {}, {})", texture, source, position, tint);
    DrawTextureRec(texture, source, position, tint);
    return JS_DupValue(js, this_val);
}

static auto texture_pro(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.texturePro/{}", argc);
    const auto args = js::unpack_args<Texture, Rectangle, Rectangle, Vector2, float, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture, source, dest, origin, rotation, tint] = *args;
    SPDLOG_TRACE("DrawTexturePro({}, {}, {}, {}, {}, {})", texture, source, dest, origin, rotation, tint);
    DrawTexturePro(texture, source, dest, origin, rotation, tint);
    return JS_DupValue(js, this_val);
}

static auto texture_npatch(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.texturePro/{}", argc);
    const auto args = js::unpack_args<Texture, NPatchInfo, Rectangle, Vector2, float, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture, npatch, dest, origin, rotation, tint] = *args;
    SPDLOG_TRACE("DrawTextureNPatch({}, {}, {}, {}, {}, {});", texture, npatch, dest, origin, rotation, tint);
    DrawTextureNPatch(texture, npatch, dest, origin, rotation, tint);
    return JS_DupValue(js, this_val);
}

static auto text_simple(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.text/{}", argc);
    const auto args = js::unpack_args<std::string, int, int, int, Color>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [text, x, y, font_size, color] = *args;
    SPDLOG_TRACE("DrawText('{}', {}, {}, {}, {})", text, x, y, font_size, color);
    DrawText(text.c_str(), x, y, font_size, color);

    return JS_DupValue(js, this_val);
}

static auto text_pro(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.textPro/{}", argc);
    if (argc != 1) {
        return JS_ThrowTypeError(js, "graphics.textPro expects 1 argument, but %d were provided", argc);
    }

    auto text = text_from_value(js, argv[0]);
    if (!text) return JS_Throw(js, text.error());

    const auto font_size = text->font_size;
    const auto position = text->position;
    const auto color = text->color;
    const auto origin = text->origin.value_or(Vector2 {});
    const auto rotation = text->rotation.value_or(0);
    const auto spacing = text->spacing.value_or(0);
    const auto font = text->font.value_or(GetFontDefault());

    if (const auto str = std::get_if<std::string>(&text->text)) {
        SPDLOG_TRACE("DrawTextPro({}, '{}', {}, {}, {})", font, *str, position, font_size, color);
        DrawTextPro(font, str->c_str(), position, origin, rotation, font_size, spacing, color);

    } else if (const auto codepoint = std::get_if<int>(&text->text)) {
        SPDLOG_TRACE("DrawTextCodepoint(font, {}, {}, {}, {})", font, *codepoint, position, font_size, color);
        DrawTextCodepoint(font, *codepoint, position, font_size, color);

    } else if (const auto codepoints = std::get_if<std::vector<int>>(&text->text)) {
        SPDLOG_TRACE(
            "DrawTextCodepoints({}, {}, {}, {}, {}, {})",
            font,
            (void *)codepoints->data(),
            codepoints->size(),
            position,
            font_size,
            color
        );
        DrawTextCodepoints(font, codepoints->data(), int(codepoints->size()), position, font_size, spacing, color);
    }

    return JS_DupValue(js, this_val);
}

static auto begin_texture_mode(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.beginTextureMode/{}", argc);
    const auto args = js::unpack_args<RenderTexture>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture] = *args;
    SPDLOG_TRACE("BeginTextureMode({})", texture);
    BeginTextureMode(texture);
    return JS_DupValue(js, this_val);
}

static auto end_texture_mode(JSContext *js, JSValueConst this_val, int argc, JSValueConst *) -> JSValue {
    SPDLOG_TRACE("graphics.endTextureMode/{}", argc);
    SPDLOG_TRACE("EndTextureMode()");
    EndTextureMode();
    return JS_DupValue(js, this_val);
}

static auto with_texture(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.withTexture/{}", argc);
    const auto args = js::unpack_args<RenderTexture, JSValue>(js, argc, argv);
    if (!args) return JS_Throw(js, args.error());
    const auto [texture, function] = *args;

    SPDLOG_TRACE("BeginTextureMode({})", texture);
    BeginTextureMode(texture);
    auto ret = JS_Call(js, function, JS_UNDEFINED, 0, nullptr);
    SPDLOG_TRACE("EndTextureMode()");
    EndTextureMode();
    if (JS_IsException(ret)) {
        return ret;
    }
    JS_FreeValue(js, ret);

    return JS_DupValue(js, this_val);
}

static const auto FUNCS = std::array {
    JSCFunctionListEntry JS_CFUNC_DEF("clear", 1, clear),
    JSCFunctionListEntry JS_CFUNC_DEF("circle", 4, circle_simple),
    JSCFunctionListEntry JS_CFUNC_DEF("rectangle", 5, rectangle_simple),
    JSCFunctionListEntry JS_CFUNC_DEF("rectangleV", 3, rectangle_v),
    JSCFunctionListEntry JS_CFUNC_DEF("rectangleRec", 2, rectangle_rec),
    JSCFunctionListEntry JS_CFUNC_DEF("rectanglePro", 4, rectangle_pro),
    JSCFunctionListEntry JS_CFUNC_DEF("beginCameraMode", 1, begin_camera_mode),
    JSCFunctionListEntry JS_CFUNC_DEF("endCameraMode", 0, end_camera_mode),
    JSCFunctionListEntry JS_CFUNC_DEF("texture", 4, texture_simple),
    JSCFunctionListEntry JS_CFUNC_DEF("textureV", 3, texture_v),
    JSCFunctionListEntry JS_CFUNC_DEF("textureEx", 5, texture_ex),
    JSCFunctionListEntry JS_CFUNC_DEF("textureRec", 4, texture_rec),
    JSCFunctionListEntry JS_CFUNC_DEF("texturePro", 6, texture_pro),
    JSCFunctionListEntry JS_CFUNC_DEF("textureNPatch", 6, texture_npatch),
    JSCFunctionListEntry JS_CFUNC_DEF("text", 5, text_simple),
    JSCFunctionListEntry JS_CFUNC_DEF("textPro", 1, text_pro),
    JSCFunctionListEntry JS_CFUNC_DEF("beginTextureMode", 1, begin_texture_mode),
    JSCFunctionListEntry JS_CFUNC_DEF("endTextureMode", 0, end_texture_mode),
    JSCFunctionListEntry JS_CFUNC_DEF("withTexture", 2, with_texture),
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:graphics", [](auto js, auto m) -> int {
        auto o = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, o, FUNCS.data(), int {FUNCS.size()});
        JS_SetModuleExport(js, m, "default", o);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::graphics

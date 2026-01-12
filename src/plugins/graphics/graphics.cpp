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
    ::Vector2 position = {};
    ::Color color = {};
    std::optional<::Vector2> origin = std::nullopt;
    std::optional<float> rotation = std::nullopt;
    std::optional<float> spacing = std::nullopt;
    std::optional<::Font> font = std::nullopt;
};

static auto text_from_value(::JSContext *js, ::JSValueConst value) -> std::expected<Text, JSValue> {
    auto text = Text {};

    if (auto t = js::try_get_property<std::string>(js, value, "text")) {
        text.text = *t;
    } else if (auto c = js::try_get_property<int>(js, value, "codepoint")) {
        text.text = *c;
    } else if (auto cs = js::try_get_property<std::vector<int>>(js, value, "codepoints")) {
        text.text = *cs;
    } else {
        return std::unexpected(
            ::JS_NewTypeError(
                js,
                "Text object must either have `text` (string), `codepoint` (number) or `codepoints` (number[]) property"
            )
        );
    }

    if (auto font_size = js::try_get_property<float>(js, value, "fontSize")) {
        text.font_size = *font_size;
    } else {
        return std::unexpected(font_size.error());
    }

    if (auto position = js::try_get_property<::Vector2>(js, value, "position")) {
        text.position = *position;
    } else {
        return std::unexpected(position.error());
    }

    if (auto color = js::try_get_property<::Color>(js, value, "color")) {
        text.color = *color;
    } else {
        return std::unexpected(color.error());
    }

    if (auto origin = js::try_get_property<std::optional<::Vector2>>(js, value, "origin")) {
        text.origin = *origin;
    } else {
        return std::unexpected(origin.error());
    }

    if (auto rotation = js::try_get_property<std::optional<float>>(js, value, "rotation")) {
        text.rotation = *rotation;
    } else {
        return std::unexpected(rotation.error());
    }

    if (auto spacing = js::try_get_property<std::optional<float>>(js, value, "spacing")) {
        text.spacing = *spacing;
    } else {
        return std::unexpected(spacing.error());
    }

    if (auto font = js::try_get_property<std::optional<::Font>>(js, value, "font")) {
        text.font = *font;
    } else {
        return std::unexpected(font.error());
    }

    return text;
}

static auto clear(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.clear/{}", argc);
    if (argc != 1) {
        JS_ThrowTypeError(js, "graphics.clear expects 1 argument, but %d were provided", argc);
    }

    const auto color = color::from_value(js, argv[0]);
    if (!color.has_value()) {
        return color.error();
    }

    SPDLOG_TRACE("ClearBackground({})", color::to_string(*color));
    ClearBackground(*color);
    return JS_DupValue(js, this_val);
}

static auto circle_simple(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.circle/{}", argc);
    if (argc != 4) {
        JS_ThrowTypeError(js, "graphics.circle expects 4 argument, but %d were provided", argc);
    }

    auto x = int {}, y = int {};
    auto radius = double {};
    JS_ToInt32(js, &x, argv[0]);
    JS_ToInt32(js, &y, argv[1]);
    JS_ToFloat64(js, &radius, argv[2]);
    const auto color = color::from_value(js, argv[3]);
    if (!color.has_value()) {
        return color.error();
    }
    SPDLOG_TRACE("DrawCircle({}, {}, {}, {})", x, y, radius, color::to_string(*color));
    DrawCircle(x, y, float(radius), *color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_simple(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.rectangle/{}", argc);
    if (argc != 5) {
        return JS_ThrowTypeError(js, "graphics.rectangle accepts 5 arguments, but %d were provided", argc);
    }

    auto x = int {}, y = int {}, width = int {}, height = int {};
    ::JS_ToInt32(js, &x, argv[0]);
    ::JS_ToInt32(js, &y, argv[1]);
    ::JS_ToInt32(js, &width, argv[2]);
    ::JS_ToInt32(js, &height, argv[3]);
    const auto color = color::from_value(js, argv[4]);
    if (!color.has_value()) {
        return color.error();
    }

    SPDLOG_TRACE("DrawRectangle({}, {}, {}, {}, {})", x, y, width, height, color::to_string(*color));
    DrawRectangle(x, y, width, height, *color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_v(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.rectangleV/{}", argc);
    if (argc != 3) {
        return JS_ThrowTypeError(js, "graphics.rectangleV accepts 3 arguments, but %d were provided", argc);
    }

    const auto position = math::vector2::from_value(js, argv[0]);
    if (!position.has_value()) {
        return position.error();
    }
    const auto size = math::vector2::from_value(js, argv[1]);
    if (!size.has_value()) {
        return size.error();
    }
    const auto color = color::from_value(js, argv[2]);
    if (!color.has_value()) {
        return color.error();
    }

    SPDLOG_TRACE(
        "DrawRectangleV({}, {}, {})",
        math::vector2::to_string(*position),
        math::vector2::to_string(*size),
        color::to_string(*color)
    );
    ::DrawRectangleV(*position, *size, *color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_rec(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.rectangleRec/{}", argc);
    if (argc != 2) {
        return JS_ThrowTypeError(js, "graphics.rectangleRec accepts 2 arguments, but %d were provided", argc);
    }

    const auto rec = math::rectangle::from_value(js, argv[0]);
    if (!rec.has_value()) {
        return rec.error();
    }
    const auto color = color::from_value(js, argv[1]);
    if (!color.has_value()) {
        return color.error();
    }

    SPDLOG_TRACE("DrawRectangleRec({}, {})", math::rectangle::to_string(*rec), color::to_string(*color));
    DrawRectangleRec(*rec, *color);
    return JS_DupValue(js, this_val);
}

static auto rectangle_pro(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.rectanglePro/{}", argc);
    if (argc != 4) {
        return JS_ThrowTypeError(js, "graphics.rectanglePro accepts 4 arguments, but %d were provided", argc);
    }

    const auto rec = math::rectangle::from_value(js, argv[0]);
    if (!rec.has_value()) {
        return rec.error();
    }
    const auto origin = math::vector2::from_value(js, argv[1]);
    if (!origin.has_value()) {
        return origin.error();
    }
    auto rotation = double {};
    ::JS_ToFloat64(js, &rotation, argv[2]);
    const auto color = color::from_value(js, argv[3]);
    if (!color.has_value()) {
        return color.error();
    }

    SPDLOG_TRACE(
        "DrawRectanglePro({}, {}, {}, {})",
        math::rectangle::to_string(*rec),
        math::vector2::to_string(*origin),
        rotation,
        color::to_string(*color)
    );
    ::DrawRectanglePro(*rec, *origin, static_cast<float>(rotation), *color);
    return JS_DupValue(js, this_val);
}

static auto begin_camera_mode(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.beginCameraMode/{}", argc);
    if (argc != 4) {
        return JS_ThrowTypeError(js, "graphics.beginCameraMode accepts 1 argument, but %d were provided", argc);
    }

    const auto camera = camera::from_value(js, argv[0]);
    if (!camera.has_value()) {
        return camera.error();
    }

    SPDLOG_TRACE("BeginMode2D({})", camera::to_string(*camera));
    ::BeginMode2D(*camera);
    return JS_DupValue(js, this_val);
}

static auto end_camera_mode(JSContext *js, JSValueConst this_val, int argc, JSValueConst *) -> JSValue {
    SPDLOG_TRACE("graphics.beginCameraMode/{}", argc);
    if (argc != 0) {
        return JS_ThrowTypeError(js, "graphics.beginCameraMode accepts no arguments, but %d were provided", argc);
    }

    SPDLOG_TRACE("BeginMode2D()");
    ::EndMode2D();
    return JS_DupValue(js, this_val);
}

static auto texture_simple(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.texture/{}", argc);
    if (argc != 4) {
        return JS_ThrowTypeError(js, "graphics.texture accepts 4 arguments, but %d were provided", argc);
    }

    const auto texture = texture::from_value(js, argv[0]);
    if (!texture.has_value()) {
        return texture.error();
    }
    auto x = int {}, y = int {};
    ::JS_ToInt32(js, &x, argv[1]);
    ::JS_ToInt32(js, &y, argv[2]);
    const auto tint = color::from_value(js, argv[3]);
    if (!tint.has_value()) {
        return tint.error();
    }

    SPDLOG_TRACE("DrawTexture(<texture>, {}, {}, {})", x, y, color::to_string(*tint));
    ::DrawTexture(*texture, x, y, *tint);
    return JS_DupValue(js, this_val);
}

static auto texture_v(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.textureV/{}", argc);
    if (argc != 3) {
        return JS_ThrowTypeError(js, "graphics.textureV accepts 3 arguments, but %d were provided", argc);
    }

    const auto texture = texture::from_value(js, argv[0]);
    if (!texture.has_value()) {
        return texture.error();
    }
    const auto position = math::vector2::from_value(js, argv[1]);
    if (!position.has_value()) {
        return position.error();
    }
    const auto tint = color::from_value(js, argv[2]);
    if (!tint.has_value()) {
        return tint.error();
    }

    SPDLOG_TRACE("DrawTextureV(<texture>, {}, {})", math::vector2::to_string(*position), color::to_string(*tint));
    ::DrawTextureV(*texture, *position, *tint);
    return JS_DupValue(js, this_val);
}

static auto texture_ex(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.textureEx/{}", argc);
    if (argc != 5) {
        return JS_ThrowTypeError(js, "graphics.textureEx accepts 5 arguments, but %d were provided", argc);
    }

    const auto texture = texture::from_value(js, argv[0]);
    if (!texture.has_value()) {
        return texture.error();
    }
    const auto position = math::vector2::from_value(js, argv[1]);
    if (!position.has_value()) {
        return position.error();
    }
    auto rotation = double {}, scale = double {};
    ::JS_ToFloat64(js, &rotation, argv[2]);
    ::JS_ToFloat64(js, &scale, argv[3]);
    const auto tint = color::from_value(js, argv[4]);
    if (!tint.has_value()) {
        return tint.error();
    }

    SPDLOG_TRACE(
        "DrawTextureEx(<texture>, {}, {}, {}, {})",
        math::vector2::to_string(*position),
        rotation,
        scale,
        color::to_string(*tint)
    );
    DrawTextureEx(*texture, *position, float(rotation), float(scale), *tint);
    return JS_DupValue(js, this_val);
}

static auto texture_rec(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.textureRec/{}", argc);
    if (argc != 4) {
        return JS_ThrowTypeError(js, "graphics.textureRec accepts 4 arguments, but %d were provided", argc);
    }

    const auto texture = texture::from_value(js, argv[0]);
    if (!texture.has_value()) {
        return texture.error();
    }
    const auto source = math::rectangle::from_value(js, argv[1]);
    if (!source.has_value()) {
        return source.error();
    }
    const auto position = math::vector2::from_value(js, argv[2]);
    if (!position.has_value()) {
        return position.error();
    }
    const auto tint = color::from_value(js, argv[3]);
    if (!tint.has_value()) {
        return tint.error();
    }

    SPDLOG_TRACE(
        "DrawTextureRec(<texture>, {}, {}, {})",
        math::rectangle::to_string(*source),
        math::vector2::to_string(*position),
        color::to_string(*tint)
    );
    ::DrawTextureRec(*texture, *source, *position, *tint);
    return JS_DupValue(js, this_val);
}

static auto texture_pro(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    SPDLOG_TRACE("graphics.texturePro/{}", argc);
    if (argc != 6) {
        return JS_ThrowTypeError(js, "graphics.texturePro accepts 6 arguments, but %d were provided", argc);
    }

    const auto texture = texture::from_value(js, argv[0]);
    if (!texture.has_value()) {
        return texture.error();
    }
    const auto source = math::rectangle::from_value(js, argv[1]);
    if (!source.has_value()) {
        return source.error();
    }
    const auto dest = math::rectangle::from_value(js, argv[2]);
    if (!dest.has_value()) {
        return dest.error();
    }
    const auto origin = math::vector2::from_value(js, argv[3]);
    if (!origin.has_value()) {
        return origin.error();
    }
    auto rotation = double {};
    JS_ToFloat64(js, &rotation, argv[4]);
    const auto tint = color::from_value(js, argv[5]);
    if (!tint.has_value()) {
        return tint.error();
    }

    SPDLOG_TRACE(
        "DrawTexturePro(<texture>, {}, {}, {}, {}, {})",
        math::rectangle::to_string(*source),
        math::rectangle::to_string(*dest),
        math::vector2::to_string(*origin),
        rotation,
        color::to_string(*tint)
    );
    DrawTexturePro(*texture, *source, *dest, *origin, float(rotation), *tint);
    return JS_DupValue(js, this_val);
}

static auto texture_npatch(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.texturePro/{}", argc);
    if (argc != 6) {
        return JS_ThrowTypeError(js, "graphics.texturePro accepts 6 arguments, but %d were provided", argc);
    }

    const auto texture = texture::from_value(js, argv[0]);
    if (!texture.has_value()) {
        return texture.error();
    }
    const auto npatch = npatch::from_value(js, argv[1]);
    if (!npatch.has_value()) {
        return npatch.error();
    }
    const auto dest = math::rectangle::from_value(js, argv[2]);
    if (!dest.has_value()) {
        return dest.error();
    }
    const auto origin = math::vector2::from_value(js, argv[3]);
    if (!origin.has_value()) {
        return origin.error();
    }
    auto rotation = double {};
    JS_ToFloat64(js, &rotation, argv[4]);
    const auto tint = color::from_value(js, argv[5]);
    if (!tint.has_value()) {
        return tint.error();
    }

    // TODO: trace DrawTextureNPatch
    ::DrawTextureNPatch(*texture, *npatch, *dest, *origin, static_cast<float>(rotation), *tint);
    return JS_DupValue(js, this_val);
}

static auto text_simple(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.text/{}", argc);
    if (argc != 5) {
        return JS_ThrowTypeError(js, "graphics.text expects 5 arguments but %d were provided", argc);
    }

    auto text = js::try_as<std::string>(js, argv[0]);
    if (!text)
        return JS_Throw(js, text.error());
    auto x = js::try_as<int>(js, argv[1]);
    if (!x)
        return JS_Throw(js, x.error());
    auto y = js::try_as<int>(js, argv[2]);
    if (!y)
        return JS_Throw(js, y.error());
    auto font_size = js::try_as<int>(js, argv[3]);
    if (!font_size)
        return JS_Throw(js, font_size.error());
    auto color = js::try_as<::Color>(js, argv[4]);
    if (!color)
        return JS_Throw(js, color.error());

    SPDLOG_TRACE("DrawText('{}', {}, {}, {}, {})", *text, *x, *y, *font_size, color::to_string(*color));
    DrawText(text->c_str(), *x, *y, *font_size, *color);

    return JS_DupValue(js, this_val);
}

static auto text_pro(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    SPDLOG_TRACE("graphics.textPro/{}", argc);
    if (argc != 1) {
        return JS_ThrowTypeError(js, "graphics.textPro expects 1 argument, but %d were provided", argc);
    }

    auto text = text_from_value(js, argv[0]);
    if (!text)
        return JS_Throw(js, text.error());

    const auto font_size = text->font_size;
    const auto position = text->position;
    const auto color = text->color;
    const auto origin = text->origin.value_or(Vector2 {});
    const auto rotation = text->rotation.value_or(0);
    const auto spacing = text->spacing.value_or(0);
    const auto font = text->font.value_or(GetFontDefault());

    if (const auto str = std::get_if<std::string>(&text->text)) {
        SPDLOG_TRACE(
            "DrawTextPro(<Font>, '{}', {}, {}, {})",
            *str,
            math::vector2::to_string(position),
            font_size,
            color::to_string(color)
        );
        DrawTextPro(font, str->c_str(), position, origin, rotation, font_size, spacing, color);

    } else if (const auto codepoint = std::get_if<int>(&text->text)) {
        SPDLOG_TRACE(
            "DrawTextCodepoint(<Font>, {}, {}, {}, {})",
            *codepoint,
            math::vector2::to_string(position),
            font_size,
            color::to_string(color)
        );
        DrawTextCodepoint(font, *codepoint, position, font_size, color);

    } else if (const auto codepoints = std::get_if<std::vector<int>>(&text->text)) {
        SPDLOG_TRACE(
            "DrawTextCodepoints(<Font>, {}, {}, {}, {}, {})",
            (void *)codepoints->data(),
            codepoints->size(),
            math::vector2::to_string(position),
            font_size,
            color::to_string(color)
        );
        DrawTextCodepoints(font, codepoints->data(), int(codepoints->size()), position, font_size, spacing, color);
    }

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
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:graphics", [](auto js, auto m) -> int {
        auto o = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, o, FUNCS.data(), int{FUNCS.size()});
        JS_SetModuleExport(js, m, "default", o);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::plugins::graphics

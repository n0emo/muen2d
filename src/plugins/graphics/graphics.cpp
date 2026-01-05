#include <plugins/graphics.hpp>

#include <array>

#include <raylib.h>

#include <plugins/math.hpp>

namespace muen::plugins::graphics {

static auto clear(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    const auto color = color::from_value(js, argv[0]);
    if (!color.has_value()) {
        return color.error();
    }
    ClearBackground(*color);
    return JS_UNDEFINED;
}

static auto circle_simple(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    auto x = int {}, y = int {};
    auto radius = double {};
    JS_ToInt32(js, &x, argv[0]);
    JS_ToInt32(js, &y, argv[1]);
    JS_ToFloat64(js, &radius, argv[2]);
    const auto color = color::from_value(js, argv[3]);
    if (!color.has_value()) {
        return color.error();
    }
    DrawCircle(x, y, float(radius), *color);
    return JS_UNDEFINED;
}

static auto rectangle_simple(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    auto x = int {}, y = int {}, width = int {}, height = int {};
    ::JS_ToInt32(js, &x, argv[0]);
    ::JS_ToInt32(js, &y, argv[1]);
    ::JS_ToInt32(js, &width, argv[2]);
    ::JS_ToInt32(js, &height, argv[3]);
    const auto color = color::from_value(js, argv[4]);
    if (!color.has_value()) {
        return color.error();
    }
    ::DrawRectangle(x, y, width, height, *color);
    return JS_UNDEFINED;
}

static auto rectangle_v(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
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
    ::DrawRectangleV(*position, *size, *color);
    return JS_UNDEFINED;
}

static auto rectangle_rec(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    const auto rec = math::rectangle::from_value(js, argv[0]);
    if (!rec.has_value()) {
        return rec.error();
    }
    const auto color = color::from_value(js, argv[1]);
    if (!color.has_value()) {
        return color.error();
    }
    ::DrawRectangleRec(*rec, *color);
    return JS_UNDEFINED;
}

static auto rectangle_pro(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
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
    ::DrawRectanglePro(*rec, *origin, static_cast<float>(rotation), *color);
    return JS_UNDEFINED;
}

static auto begin_camera_mode(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
    const auto camera = camera::from_value(js, argv[0]);
    if (!camera.has_value()) {
        return camera.error();
    }
    ::BeginMode2D(*camera);
    return JS_UNDEFINED;
}

static auto end_camera_mode(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
    ::EndMode2D();
    return JS_UNDEFINED;
}

static auto texture_simple(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
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
    ::DrawTexture(*texture, x, y, *tint);
    return JS_UNDEFINED;
}

static auto texture_v(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
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
    ::DrawTextureV(*texture, *position, *tint);
    return JS_UNDEFINED;
}

static auto texture_ex(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
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
    ::DrawTextureEx(*texture, *position, float(rotation), float(scale), *tint);
    return JS_UNDEFINED;
}

static auto texture_rec(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
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
    ::DrawTextureRec(*texture, *source, *position, *tint);
    return JS_UNDEFINED;
}

static auto texture_pro(::JSContext *js, ::JSValueConst this_val, int argc, ::JSValueConst *argv) -> ::JSValue {
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
    ::DrawTexturePro(*texture, *source, *dest, *origin, float(rotation), *tint);
    return JS_UNDEFINED;
}

static auto texture_npatch(JSContext *js, JSValueConst this_val, int argc, JSValueConst *argv) -> JSValue {
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
    ::DrawTextureNPatch(*texture, *npatch, *dest, *origin, static_cast<float>(rotation), *tint);
    return JS_UNDEFINED;
}

static const auto funcs = std::array {
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
};

auto module(JSContext *js) -> JSModuleDef * {
    auto m = JS_NewCModule(js, "muen:graphics", [](auto js, auto m) -> int {
        auto o = JS_NewObject(js);
        JS_SetPropertyFunctionList(js, o, funcs.data(), funcs.size());
        JS_SetModuleExport(js, m, "default", o);

        return 0;
    });

    JS_AddModuleExport(js, m, "default");

    return m;
}

} // namespace muen::bindings::graphics

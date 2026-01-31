#pragma once

#include <spdlog/spdlog.h>

#include <engine/plugin.hpp>
#include <quickjs.hpp>
#include <raylib.hpp>

namespace glint::plugins::math {

using namespace js;

auto plugin(JSContext *js) -> EnginePlugin;

namespace vector2 {
    extern const JSClassDef VECTOR2;
    auto module(JSContext *js) -> ::JSModuleDef *;
    auto to_string(Vector2 vec) -> std::string;
} // namespace vector2

class JSRectangle: public JSClass<JSRectangle> {
  public:
    static auto zero(JSContext *ctx, JSValueConst this_val) noexcept -> JSValue {
        return JSRectangle::create_instance_this(borrow(ctx, this_val), 0.0f, 0.0f, 0.0f, 0.0f);
    }

    auto get_rec() noexcept -> ::Rectangle { return _rec; }

    auto get_x() noexcept -> float { return _rec.x; }

    auto set_x(float val) noexcept -> void { _rec.x = val; }

    auto get_y() noexcept -> float { return _rec.y; }

    auto set_y(float val) noexcept -> void { _rec.y = val; }

    auto get_width() noexcept -> float { return _rec.width; }

    auto set_width(float val) noexcept -> void { _rec.width = val; }

    auto get_height() noexcept -> float { return _rec.height; }

    auto set_height(float val) noexcept -> void { _rec.height = val; }

    auto to_string() noexcept -> std::string { return fmt::format("{}", _rec); }

    // JSClass implementation

    constexpr static auto class_name = "Rectangle";

    constexpr static auto static_properties = PropertyList {
        export_static_method<&JSRectangle::zero>("zero"),
    };

    constexpr static auto instance_properties = PropertyList {
        export_getset<&JSRectangle::get_x, &JSRectangle::set_x>("x"),
        export_getset<&JSRectangle::get_y, &JSRectangle::set_y>("y"),
        export_getset<&JSRectangle::get_width, &JSRectangle::set_width>("width"),
        export_getset<&JSRectangle::get_height, &JSRectangle::set_height>("height"),
        export_method<&JSRectangle::to_string>("toString"),
    };

    auto initialize(float x, float y, float width, float height) noexcept {
        _rec.x = x;
        _rec.y = y;
        _rec.width = width;
        _rec.height = height;
    }

  private:
    ::Rectangle _rec {};
};

inline auto rectangle_module(JSContext *ctx) -> JSModuleDef * {
    auto m = JS_NewCModule(ctx, "glint:Rectangle", [](auto ctx, auto m) -> int {
        auto ctor = JSRectangle::define(ctx).take();
        JS_SetModuleExport(ctx, m, "default", ctor);
        return 0;
    });

    JS_AddModuleExport(ctx, m, "default");
    return m;
}

} // namespace glint::plugins::math

namespace glint::js {

template<>
auto convert_from_js<Vector2>(const Value& v) noexcept -> JSResult<Vector2>;

template<>
inline auto convert_from_js<plugins::math::JSRectangle *>(const Value& val) noexcept
    -> JSResult<plugins::math::JSRectangle *> {
    return plugins::math::JSRectangle::get_instance(val);
}

template<>
inline auto convert_from_js<Rectangle>(const Value& val) noexcept -> JSResult<Rectangle> {
    if (auto r = convert_from_js<plugins::math::JSRectangle *>(val)) return (*r)->get_rec();

    auto rec = ::Rectangle {};
    auto obj = Object::from_value(val);
    if (!obj) return Unexpected(obj.error());

    if (auto v = obj->at<float>("x")) rec.x = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("y")) rec.y = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("width")) rec.width = *v;
    else return Unexpected(v.error());
    if (auto v = obj->at<float>("height")) rec.height = *v;
    else return Unexpected(v.error());

    return rec;
}

} // namespace glint::js

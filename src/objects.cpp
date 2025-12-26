#include "objects.hpp"

#include <cstring>
#include <sstream>
#include <fstream>

#include <mujs.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

#include "jsutil.h"
#include "engine.hpp"

enum class LogLevel { Trace, Debug, Info, Warn, Error };

extern "C" {
void muen_log_trace(js_State *j);
void muen_log_debug(js_State *j);
void muen_log_info(js_State *j);
void muen_log_warn(js_State *j);
void muen_log_error(js_State *j);

void muen_fs_read(js_State *j);
void muen_fs_load(js_State *j);

void muen_screen_dt(js_State *j);
void muen_screen_width(js_State *j);
void muen_screen_height(js_State *j);

void muen_graphics_clear(js_State *j);
void muen_graphics_circle(js_State *j);
void muen_graphics_rectangle(js_State *j);
void muen_graphics_begin_camera_mode(js_State *j);
void muen_graphics_end_camera_mode(js_State *j);
}

Color js_tocolor(js_State *j, int idx);
Vector2 js_tovector2(js_State *j, int idx);
Rectangle js_torectangle(js_State *j, int idx);
Camera2D js_tocamera(js_State *j, int idx);

namespace objects {

void define(mujs::Js& js) {
    js.object("console")
        .define_method(muen_log_trace, "trace", 0, mujs::READONLY)
        .define_method(muen_log_debug, "debug", 0, mujs::READONLY)
        .define_method(muen_log_info, "log", 0, mujs::READONLY)
        .define_method(muen_log_warn, "warn", 0, mujs::READONLY)
        .define_method(muen_log_error, "error", 0, mujs::READONLY)
        .set_global("console");

    js.object("fs")
        .define_method(muen_fs_read, "read", 1, mujs::READONLY)
        .define_method(muen_fs_load, "load", 1, mujs::READONLY)
        .set_global("fs");

    js.object("screen")
        .define_accessor(muen_screen_dt, nullptr, "dt", mujs::READONLY)
        .define_accessor(muen_screen_width, nullptr, "width", mujs::READONLY)
        .define_accessor(muen_screen_height, nullptr, "height", mujs::READONLY)
        .set_global("screen");

    js.object("graphics")
        .define_method(muen_graphics_clear, "clear", 1, mujs::READONLY)
        .define_method(muen_graphics_circle, "circle", 4, mujs::READONLY)
        .define_method(muen_graphics_rectangle, "rectangle", 0, mujs::READONLY)
        .define_method(muen_graphics_begin_camera_mode, "beginCameraMode", 1, mujs::READONLY)
        .define_method(muen_graphics_end_camera_mode, "endCameraMode", 0, mujs::READONLY)
        .set_global("graphics");

    js.eval_string(
        (char[]){
#include "muen.js.h"
        },
        "muen.js"
    );
    js.pop(1);
}

} // namespace objects

auto muen_log(js_State *J, LogLevel level) -> void {
    auto top = js_gettop(J);

    auto buf = std::stringstream {};
    for (auto i = 1; i < top; ++i) {
        buf << js_tostring(J, i);
        if (i < top - 1) {
            buf << " ";
        }
    }

    auto msg = buf.str();
    switch (level) {
        case LogLevel::Trace:
            spdlog::trace("{}", msg);
            break;
        case LogLevel::Debug:
            spdlog::debug("{}", msg);
            break;
        case LogLevel::Info:
            spdlog::info("{}", msg);
            break;
        case LogLevel::Warn:
            spdlog::warn("{}", msg);
            break;
        case LogLevel::Error:
            spdlog::error("{}", msg);
            break;
    }

    js_pushundefined(J);
}

void muen_log_trace(js_State *j) {
    muen_log(j, LogLevel::Trace);
}

void muen_log_debug(js_State *j) {
    muen_log(j, LogLevel::Debug);
}

void muen_log_info(js_State *j) {
    muen_log(j, LogLevel::Info);
}

void muen_log_warn(js_State *j) {
    muen_log(j, LogLevel::Warn);
}

void muen_log_error(js_State *j) {
    muen_log(j, LogLevel::Error);
}

void muen_fs_read(js_State *j) {
    const char *file_path = ::js_tostring(j, 1);

    engine::Context *ctx = static_cast<engine::Context *>(::js_getcontext(j));
    auto path = std::string {ctx->root_path};
    if (path[path.size() - 1] != '/') {
        path.push_back('/');
    }
    path.append(file_path);
    spdlog::info("{}", path);

    try {
        auto file = std::ifstream {path};
        file.exceptions(file.failbit | file.badbit);
        auto buf = std::stringstream {};
        buf << file.rdbuf();
        auto str = buf.str();
        ::js_pushstring(j, str.c_str());
    } catch (std::exception& e) {
        ::js_error(j, "Could not read %s: %s", file_path, strerror(errno));
    }
}

void muen_fs_load(js_State *j) {
    const char *file_path = ::js_tostring(j, 1);

    engine::Context *ctx = static_cast<engine::Context *>(::js_getcontext(j));
    auto path = std::string {ctx->root_path};
    if (path[path.size() - 1] != '/') {
        path.push_back('/');
    }
    path.append(file_path);
    spdlog::trace("Loading {}", path);

    try {
        auto file = std::ifstream {path};
        file.exceptions(file.failbit | file.badbit);
        auto buf = std::stringstream {};
        buf << file.rdbuf();
        auto str = buf.str();
        ::js_pop(j, 2);
        ::js_pushnull(j);
        ::js_pushstring(j, "module");
        ::js_pushstring(j, str.c_str());
        ::js_create_function(j, path.c_str());

    } catch (std::exception& e) {
        ::js_error(j, "Could not read %s: %s", file_path, strerror(errno));
    }
}

void muen_screen_dt(js_State *j) {
    auto dt = GetFrameTime();
    js_pushnumber(j, static_cast<double>(dt));
}

void muen_screen_width(js_State *j) {
    auto w = GetScreenWidth();
    js_pushnumber(j, static_cast<double>(w));
}

void muen_screen_height(js_State *j) {
    auto h = GetScreenHeight();
    js_pushnumber(j, static_cast<double>(h));
}

void muen_graphics_clear(js_State *j) {
    Color c = js_tocolor(j, 1);
    ClearBackground(c);
    js_pushundefined(j);
}

void muen_graphics_circle(js_State *j) {
    int x = js_tointeger(j, 1);
    int y = js_tointeger(j, 2);
    int radius = js_tointeger(j, 3);
    Color color = js_tocolor(j, 4);
    DrawCircle(x, y, radius, color);
    js_pushundefined(j);
}

void muen_graphics_rectangle(js_State *j) {
    Rectangle rec {};
    Vector2 origin {};
    float rotation {};
    Color color {};

    int top = js_gettop(j);

    if (top == 6) { // DrawRectangle
        rec.x = js_tointeger(j, 1);
        rec.y = js_tointeger(j, 2);
        rec.width = js_tointeger(j, 3);
        rec.height = js_tointeger(j, 4);
        color = js_tocolor(j, 5);
    } else if (top == 4) { // DrawRectangleV
        Vector2 position = js_tovector2(j, 1);
        Vector2 size = js_tovector2(j, 2);
        color = js_tocolor(j, 3);
        rec.x = position.x;
        rec.y = position.y;
        rec.width = size.x;
        rec.height = size.y;
    } else if (top == 3) { // DrawRectangleRec
        rec = js_torectangle(j, 1);
        color = js_tocolor(j, 2);
    } else if (top == 5) { // DrawRectanglePro
        rec = js_torectangle(j, 1);
        origin = js_tovector2(j, 2);
        rotation = static_cast<float>(js_tonumber(j, 3));
        color = js_tocolor(j, 4);
    } else {
        js_typeerror(j, "Invalid arguments count for graphics.rectangle");
    }

    DrawRectanglePro(rec, origin, rotation, color);

    js_pushundefined(j);
}

void muen_graphics_begin_camera_mode(js_State *j) {
    Camera2D camera {};
    camera = js_tocamera(j, 1);

    BeginMode2D(camera);
    js_pushundefined(j);
}

void muen_graphics_end_camera_mode(js_State *j) {
    EndMode2D();
    js_pushundefined(j);
}

Color js_tocolor(js_State *j, int idx) {
    Color c;

    js_getproperty(j, idx, "r");
    c.r = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "g");
    c.g = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "b");
    c.b = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "a");
    c.a = js_tointeger(j, -1);
    js_pop(j, 1);

    return c;
}

Vector2 js_tovector2(js_State *j, int idx) {
    Vector2 v;

    js_getproperty(j, idx, "x");
    v.x = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "y");
    v.y = js_tointeger(j, -1);
    js_pop(j, 1);

    return v;
}

Rectangle js_torectangle(js_State *j, int idx) {
    Rectangle r;

    js_getproperty(j, idx, "x");
    r.x = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "y");
    r.y = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "width");
    r.width = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "height");
    r.height = js_tointeger(j, -1);
    js_pop(j, 1);

    return r;
}

Camera2D js_tocamera(js_State *j, int idx) {
    Camera2D c;

    js_getproperty(j, idx, "offset");
    c.offset = js_tovector2(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "target");
    c.target = js_tovector2(j, -1);
    js_pop(j, 1);

    js_getproperty(j, idx, "rotation");
    c.rotation = static_cast<float>(js_tonumber(j, -1));
    js_pop(j, 1);

    js_getproperty(j, idx, "zoom");
    c.zoom = static_cast<float>(js_tonumber(j, -1));
    js_pop(j, 1);

    return c;
}
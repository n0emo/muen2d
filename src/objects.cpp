#include "objects.hpp"

#include <sstream>

#include <mujs.h>
#include <raylib.h>
#include <spdlog/spdlog.h>

enum class LogLevel { Trace, Debug, Info, Warn, Error };

extern "C" {

void muen_log_trace(js_State *j);
void muen_log_debug(js_State *j);
void muen_log_info(js_State *j);
void muen_log_warn(js_State *j);
void muen_log_error(js_State *j);

void muen_screen_dt(js_State *j);
void muen_screen_width(js_State *j);
void muen_screen_height(js_State *j);

void muen_graphics_clear(js_State *j);
void muen_graphics_circle(js_State *j);
}

Color js_getcolor(js_State *j, int idx);

namespace objects {

void define(mujs::Js& js) {
    js.object("console")
        .define_method(muen_log_trace, "trace", 0, mujs::READONLY)
        .define_method(muen_log_debug, "debug", 0, mujs::READONLY)
        .define_method(muen_log_info, "log", 0, mujs::READONLY)
        .define_method(muen_log_warn, "warn", 0, mujs::READONLY)
        .define_method(muen_log_error, "error", 0, mujs::READONLY)
        .set_global("console");

    js.object("screen")
        .define_accessor(muen_screen_dt, nullptr, "dt", mujs::READONLY)
        .define_accessor(muen_screen_width, nullptr, "width", mujs::READONLY)
        .define_accessor(muen_screen_height, nullptr, "height", mujs::READONLY)
        .set_global("screen");

    js.object("graphics")
        .define_method(muen_graphics_clear, "clear", 1, mujs::READONLY)
        .define_method(muen_graphics_circle, "circle", 4, mujs::READONLY)
        .set_global("graphics");

    js.eval_string(
        R"(
        function Color() {

            function parse_byte(s, i) {
                return parseInt(s.slice(i, i + 2), 16);
            }

            function assertByte(n) {
                if ((n | 0) !== n) {
                    throw TypeError("Color byte must be integer");
                }

                if (n < 0 || n >= 256) {
                    throw TypeError("Color byte must be in range [0; 255]");
                }

                return n;
            }

            if (arguments.length == 1) {
                var str = arguments[0];
                if (typeof str !== 'string' || (str.length != 7 && str.length != 9) || str[0] != '#') {
                    throw TypeError("Invalid arguments for Color");
                }

                this.r = parse_byte(str, 1);
                this.g = parse_byte(str, 3);
                this.b = parse_byte(str, 5);
                this.a = str.length == 9 ? parse_byte(str, 7) : 255;
            } else if (arguments.length == 3 || arguments.length == 4) {
                this.r = assertByte(arguments[0]);
                this.g = assertByte(arguments[1]);
                this.b = assertByte(arguments[2]);
                this.a = arguments.length == 4 ? assertByte(arguments[3]) : 255;
            } else {
                throw TypeError("Invalid arguments for Color");
            }
        }
    )"
    );
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
    Color c = js_getcolor(j, 1);
    ClearBackground(c);
    js_pushundefined(j);
}

void muen_graphics_circle(js_State *j) {
    int x = js_tointeger(j, 1);
    int y = js_tointeger(j, 2);
    int radius = js_tointeger(j, 3);
    Color color = js_getcolor(j, 4);
    DrawCircle(x, y, radius, color);
    js_pushundefined(j);
}

Color js_getcolor(js_State *j, int idx) {
    Color c;

    js_getproperty(j, -1, "r");
    c.r = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, -1, "g");
    c.g = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, -1, "b");
    c.b = js_tointeger(j, -1);
    js_pop(j, 1);

    js_getproperty(j, -1, "a");
    c.a = js_tointeger(j, -1);
    js_pop(j, 1);

    js_pop(j, 1);

    return c;
}

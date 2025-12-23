#include "window.hpp"

#include <raylib.h>
#include <spdlog/spdlog.h>

namespace window {

void spdlog_tracelog_callback(int logLevel, const char *text, va_list args);

Window create(Config config) {
    SetTraceLogCallback(spdlog_tracelog_callback);
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(config.width, config.height, config.title);
    SetTargetFPS(config.fps);
    return Window {.config = config};
}

void close(Window& self) {
    CloseWindow();
}

bool should_close(Window& self) {
    return WindowShouldClose();
}

void begin_drawing(Window& self) {
    BeginDrawing();
}

void end_drawing(Window& self) {
    EndDrawing();
}

void spdlog_tracelog_callback(int logLevel, const char *text, va_list args) {
    constexpr auto bufLen = 128;
    char buf[bufLen];
    char *message = buf;
    bool dynamicMem = false;

    va_list args_copy;
    va_copy(args_copy, args);
    auto messageSize = vsnprintf(nullptr, 0, text, args_copy) + 1;
    va_end(args_copy);

    if (messageSize > bufLen) {
        dynamicMem = true;
        message = new char[messageSize];
    }

    vsnprintf(message, messageSize, text, args);

    switch (logLevel) {
        case LOG_TRACE:
            spdlog::trace(message);
            break;
        case LOG_DEBUG:
            spdlog::debug(message);
            break;
        case LOG_INFO:
            spdlog::info(message);
            break;
        case LOG_WARNING:
            spdlog::warn(message);
            break;
        case LOG_ERROR:
            spdlog::error(message);
            break;
        case LOG_FATAL:
            spdlog::error(message);
            break;
    }

    if (dynamicMem) {
        delete[] message;
    }
}

} // namespace window

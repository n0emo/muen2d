#include "./window.hpp"

#include <array>

#include <raylib.h>
#include <spdlog/spdlog.h>

namespace window {

void spdlog_tracelog_callback(int logLevel, const char *text, va_list args);

auto setup() -> void {
    SetTraceLogCallback(spdlog_tracelog_callback);
    SetTraceLogLevel(LOG_WARNING);
}

auto create(const Config &config) -> Window {
    InitWindow(config.width, config.height, config.title.c_str());
    SetTargetFPS(config.fps);
    return Window {.config = config};
}

auto close(Window&) -> void {
    CloseWindow();
}

auto should_close(Window&) -> bool {
    return WindowShouldClose();
}

auto begin_drawing(Window&) -> void {
    BeginDrawing();
}

auto end_drawing(Window&) -> void {
    EndDrawing();
}

auto clear(Window&) -> void {
    ClearBackground(BLACK);
}

auto draw_fps(Window&) -> void {
    DrawFPS(15, 15);
}

// NOLINTBEGIN
void spdlog_tracelog_callback(int logLevel, const char *text, va_list args) {
    constexpr auto bufLen = 128;
    auto buf = std::array<char, bufLen> {};
    char *message = buf.data();
    bool dynamicMem = false;

    va_list args_copy; // NOLINT
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
        case LOG_FATAL:
            spdlog::error(message);
            break;
        default:
            abort();
    }

    if (dynamicMem) {
        delete[] message;
    }
}

// NOLINTEND

} // namespace window

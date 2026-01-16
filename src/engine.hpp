#pragma once

#include <expected>
#include <string>
#include <unordered_map>
#include <variant>

#include <raylib.h>
#include <quickjs.h>

#include <engine/plugin.hpp>
#include <error.hpp>

namespace muen::engine {

struct Engine {
    JSRuntime *runtime;
    JSContext *js;
    std::string root_path {};
    std::unordered_map<std::string, std::string> js_modules {};
    std::unordered_map<std::string, JSModuleDef *> c_modules {};
    std::vector<std::function<auto()->Result<>>> load_callbacks {};
    std::vector<std::function<auto()->Result<>>> unload_callbacks {};
    std::vector<std::function<auto()->Result<>>> update_callbacks {};
    std::vector<std::function<auto()->Result<>>> draw_callbacks {};
};

/// Create a new engine.
[[nodiscard]]
auto create() noexcept -> std::expected<Engine *, error::Error>;

/// Destroy engine
auto destroy(Engine *window) noexcept -> void;

/// Run game using engine
[[nodiscard]]
auto run(Engine& self, std::string path) -> std::expected<std::monostate, error::Error>;

/// Register plugin to engine
auto register_plugin(Engine& self, const plugins::EnginePlugin& desc) -> void;

/// Get reference to engine from context opaque pointer
auto from_js(JSContext *js) -> Engine&;

/// Get absolute path to game resource
auto resolve_path(const Engine& self, const std::string& path) -> std::string;

} // namespace muen::engine

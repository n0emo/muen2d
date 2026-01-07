#pragma once

#include <string>
#include <unordered_map>

#include <raylib.h>
#include <quickjs.h>

#include <engine/plugin.hpp>

namespace muen::engine {

struct Engine {
    JSRuntime *runtime;
    JSContext *js;
    std::string root_path {};
    std::unordered_map<std::string, std::string> js_modules {};
    std::unordered_map<std::string, JSModuleDef *> c_modules {};
    std::vector<std::function<auto()->void>> load_callbacks {};
    std::vector<std::function<auto()->void>> unload_callbacks {};
    std::vector<std::function<auto()->void>> update_callbacks {};
    std::vector<std::function<auto()->void>> draw_callbacks {};
};

auto create() -> Engine *;
auto destroy(Engine& window) -> void;
auto run(Engine& self, const char *path) -> int;
auto register_plugin(Engine& self, const plugins::EnginePlugin& desc) -> void;
auto from_js(JSContext *js) -> Engine&;
auto resolve_path(const Engine& self, const std::string& path) -> std::string;

} // namespace muen::engine

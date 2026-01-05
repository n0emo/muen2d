#pragma once

#include <string>
#include <map>
#include <functional>

#include <quickjs.h>

namespace muen::plugins {

struct EnginePlugin {
    std::string name {};
    std::map<std::string, JSModuleDef *> c_modules {};
    std::map<std::string, std::string> js_modules {};
    std::function<auto()->void> load = nullptr;
    std::function<auto()->void> unload = nullptr;
    std::function<auto()->void> update = nullptr;
    std::function<auto()->void> draw = nullptr;
};

} // namespace muen::plugins

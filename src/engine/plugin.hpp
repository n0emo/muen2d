#pragma once

#include <string>
#include <map>
#include <functional>

#include <quickjs.h>

#include <error.hpp>

namespace muen::plugins {

struct EnginePlugin {
    std::string name {};
    std::map<std::string, JSModuleDef *> c_modules {};
    std::map<std::string, std::string> js_modules {};
    std::function<auto()->Result<>> load = nullptr;
    std::function<auto()->Result<>> unload = nullptr;
    std::function<auto()->Result<>> update = nullptr;
    std::function<auto()->Result<>> draw = nullptr;
};

} // namespace muen::plugins

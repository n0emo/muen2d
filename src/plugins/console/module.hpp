#pragma once

#include <quickjs.h>

namespace muen::plugins::console {

auto module(JSContext *js) -> JSModuleDef *;

}

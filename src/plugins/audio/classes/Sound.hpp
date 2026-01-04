#pragma once

#include <quickjs.h>

namespace muen::plugins::audio::sound_class {

auto module(::JSContext *js) -> ::JSModuleDef *;

}

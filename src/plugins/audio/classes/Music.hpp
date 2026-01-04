#pragma once

#include <quickjs.h>

namespace muen::plugins::audio::music_class {

auto module(JSContext *js) -> JSModuleDef *;

}

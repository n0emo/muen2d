#pragma once

#include <raylib.h>
#include "mujs.h"

struct js_State;

namespace muen::bindings::types {

auto tocolor(::js_State *j, int idx) -> ::Color;
auto tovector2(::js_State *j, int idx) -> ::Vector2;
auto torectangle(::js_State *j, int idx) -> ::Rectangle;
auto tocamera(::js_State *j, int idx) -> ::Camera2D;
auto totexture(::js_State *j, int idx) -> ::Texture;

} // namespace muen::bindings::types

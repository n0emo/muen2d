#include "globals.hpp"

#include "js.hpp"

#include <raylib.h>

namespace muen::bindings::globals {

auto define(js_State *j) -> void {
    mujs_catch(j);

    js::pushobject(j, js::get_global_this(j));
    js::pushliteral(j, (char[]){
#include "__init.js.h"
    });
    js::eval(j);
    js::pop(j, 2);
}

} // namespace muen::bindings::globals

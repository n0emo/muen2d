#include "globals.hpp"

#include "js.hpp"

#include <raylib.h>

namespace muen::bindings::globals {

constexpr char INIT_JS[] = {
#include "__init.js.h"
};

auto define(js_State *j) -> void {
    mujs_catch(j);

    js::pushobject(j, js::get_global_this(j));
    js::pushliteral(j, INIT_JS);
    js::eval(j);
    js::pop(j, 2);

    js::endtry(j);
}

} // namespace muen::bindings::globals

#include "bindings.hpp"

#include "bindings/console.hpp"
#include "bindings/fs.hpp"
#include "bindings/globals.hpp"
#include "bindings/graphics.hpp"
#include "bindings/music.hpp"
#include "bindings/screen.hpp"
#include "bindings/sound.hpp"
#include "bindings/texture.hpp"

#include "js.hpp"

namespace muen::bindings {

auto define_global_function(js_State *j, void (*fun)(js_State *), const char *name, int length) -> void {
    js::newcfunction(j, fun, name, length);
    js::defglobal(j, name, js::READONLY | js::DONTCONF | js::DONTENUM);
}

auto define(js::State *j) -> void {
    mujs_catch(j);

    console::define(j);
    fs::define(j);
    graphics::define(j);
    music::define(j);
    screen::define(j);
    sound::define(j);
    texture::define(j);

    globals::define(j);

    js::endtry(j);
}

} // namespace muen::bindings

using namespace muen;

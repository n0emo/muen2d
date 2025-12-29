#include "texture.hpp"

#include <raylib.h>
#include <string>

#include "bindings.hpp"
#include "bindings/types.hpp"
#include "engine.hpp"
#include "js.h"
#include "js.hpp"
#include "mujs.h"

namespace muen::bindings::texture {

auto define(js_State *j) -> void {
    define_global_function(
        j,
        [](js::State *j) -> void {
            auto file_name = js::tostring(j, 1);
            auto e = static_cast<engine::Engine *>(js::getcontext(j));
            auto path = std::string {e->root_path};
            if (path[path.size() - 1] != '/') {
                path.push_back('/');
            }
            path += file_name;

            auto texture = LoadTexture(path.c_str());

            js::newobject(j);
            js::pushnumber(j, texture.id);
            js::setproperty(j, -2, "id");
            js::pushnumber(j, texture.width);
            js::setproperty(j, -2, "width");
            js::pushnumber(j, texture.height);
            js::setproperty(j, -2, "height");
            js::pushnumber(j, texture.mipmaps);
            js::setproperty(j, -2, "mipmaps");
            js::pushnumber(j, texture.format);
            js::setproperty(j, -2, "format");
        },
        "__muenTextureLoad",
        1
    );

    define_global_function(
        j,
        [](js_State *j) -> void {
            auto t = types::totexture(j, 1);
            UnloadTexture(t);
            js::pushundefined(j);
        },
        "__muenTextureUnload",
        1
    );

    define_global_function(
        j,
        [](js_State *j) -> void {
            auto t = types::totexture(j, 1);
            auto result = IsTextureValid(t);
            js::pushboolean(j, result);
        },
        "__muenTextureIsValid",
        1
    );
}

} // namespace muen::bindings::texture
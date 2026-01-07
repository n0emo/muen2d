#include "./game.hpp"

#include <cerrno>
#include <cstring>
#include <sstream>
#include <fstream>

#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <jsutil.hpp>

namespace muen::game {

auto read_config(JSContext *js, JSValue object) -> std::expected<Config, JSValue> {
    Config config;
    if (JS_IsUndefined(object) || JS_IsNull(object)) {
        return config;
    }
    if (!JS_IsObject(object)) {
        return std::unexpected(JS_ThrowInternalError(js, "Game config must be object"));
    }

    auto title = JS_GetPropertyStr(js, object, "title");
    defer(JS_FreeValue(js, title));
    auto title_str = JS_ToCString(js, title);
    if (JS_HasException(js)) {
        return std::unexpected(JS_GetException(js));
    }
    defer(JS_FreeCString(js, title_str));
    config.title = title_str;

    // TODO: other config fields

    return config;
}

auto create(JSContext *js, std::string path) -> std::expected<Game, JSValue> {
    path += "game.js";
    auto game_file = std::ifstream {path};
    game_file.exceptions(game_file.badbit | game_file.failbit);
    auto game_buf = std::stringstream {};
    try {
        game_buf << game_file.rdbuf();
    } catch (std::exception& e) {
        return std::unexpected(JS_ThrowInternalError(js, "Could not load game file: %s", strerror(errno)));
    }

    const auto src = game_buf.str();
    auto mod = JS_Eval(
        js,
        src.c_str(),
        src.size(),
        path.c_str(),
        JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT | JS_EVAL_FLAG_COMPILE_ONLY
    );
    if (JS_HasException(js)) {
        return std::unexpected(JS_GetException(js));
    }


    auto eval_ret = JS_EvalFunction(js, mod);
    defer(JS_FreeValue(js, eval_ret));
    auto eval_result = JS_PromiseResult(js, eval_ret);
    if (JS_IsError(eval_result)) {
        return std::unexpected(eval_result);
    }
    JS_FreeValue(js, eval_result);

    auto m = static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(mod));
    auto ns = JS_GetModuleNamespace(js, m);
    defer(JS_FreeValue(js, ns));

    auto config_obj = JS_GetPropertyStr(js, ns, "config");
    if (JS_IsException(config_obj)) {
        return std::unexpected(config_obj);
    }
    defer(JS_FreeValue(js, config_obj));

    auto config = read_config(js, config_obj);
    if (!config.has_value()) {
        return std::unexpected(config.error());
    }

    auto load = JS_GetPropertyStr(js, ns, "load");
    if (!JS_IsUndefined(load) && !JS_IsNull(load) && !JS_IsFunction(js, load)) {
        JS_FreeValue(js, load);
        return std::unexpected(JS_ThrowInternalError(js, "Game load must be function"));
    }

    auto update = JS_GetPropertyStr(js, ns, "update");
    if (!JS_IsUndefined(update) && !JS_IsNull(update) && !JS_IsFunction(js, update)) {
        JS_FreeValue(js, update);
        return std::unexpected(JS_ThrowInternalError(js, "Game update must be function"));
    }

    auto draw = JS_GetPropertyStr(js, ns, "draw");
    if (!JS_IsUndefined(draw) && !JS_IsNull(draw) && !JS_IsFunction(js, draw)) {
        JS_FreeValue(js, draw);
        return std::unexpected(JS_ThrowInternalError(js, "Game draw must be function"));
    }

    return Game {
        .js = js,
        .config = *config,
        .mod = mod,
        .load = load,
        .update = update,
        .draw = draw,
    };
}

auto destroy(Game &self) -> void {
    ::JS_FreeValue(self.js, self.load);
    ::JS_FreeValue(self.js, self.update);
    ::JS_FreeValue(self.js, self.draw);
}

auto safe_call(Game &self, JSValue func) -> std::expected<std::monostate, JSValue> {
    if (!JS_IsFunction(self.js, func)) {
        return std::monostate{};
    }

    auto ret = JS_Call(self.js, func, JS_UNDEFINED, 0, nullptr);
    if (JS_IsException(ret)) {
        return std::unexpected(ret);
    }
    defer(JS_FreeValue(self.js, ret));

    return std::monostate{};
}

auto load(Game &self) -> std::expected<std::monostate, JSValue> {
    return safe_call(self, self.load);
}

auto update(Game &self) -> std::expected<std::monostate, JSValue> {
    return safe_call(self, self.update);
}

auto draw(Game &self) -> std::expected<std::monostate, JSValue> {
    return safe_call(self, self.draw);
}

} // namespace muen::game

#include "./game.hpp"

#include <array>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <span>
#include <sstream>

#include <spdlog/spdlog.h>

#include <defer.hpp>
#include <jsutil.hpp>

namespace muen::game {

auto read_config(JSContext *js, JSValue object) -> Result<Config> {
    Config config;
    if (JS_IsUndefined(object) || JS_IsNull(object)) {
        return config;
    }
    if (!JS_IsObject(object)) {
        return Err(error::create("Game config must be object"));
    }

    if (auto title = js::try_get_property<std::string>(js, object, "title")) config.title = *title;
    else return Err(error::from_js(js, title.error()));

    // TODO: other config fields

    return config;
}

auto create(JSContext *js, std::string path) -> Result<Game> {
    path += "game.js";
    auto game_file = std::ifstream {path};
    game_file.exceptions(game_file.badbit | game_file.failbit);
    auto game_buf = std::stringstream {};
    try {
        game_buf << game_file.rdbuf();
    } catch (std::exception&) {
        return Err(error::create(std::format("Could not load game file: {}", strerror(errno))));
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
        return Err(error::from_js(js, JS_GetException(js)));
    }

    auto eval_ret = JS_EvalFunction(js, mod);
    defer(JS_FreeValue(js, eval_ret));
    auto eval_result = JS_PromiseResult(js, eval_ret);
    if (JS_IsError(eval_result)) {
        return Err(error::from_js(js, eval_result));
    }
    JS_FreeValue(js, eval_result);

    auto m = static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(mod));
    auto ns = JS_GetModuleNamespace(js, m);
    defer(JS_FreeValue(js, ns));

    auto config_obj = js::try_get_property<JSValue>(js, ns, "config");
    if (!config_obj) return Err(error::from_js(js, config_obj.error()));
    defer(JS_FreeValue(js, *config_obj));

    auto config = read_config(js, *config_obj);
    if (!config.has_value()) {
        return Err(config.error());
    }

    auto load = JS_GetPropertyStr(js, ns, "load");
    if (!JS_IsUndefined(load) && !JS_IsNull(load) && !JS_IsFunction(js, load)) {
        JS_FreeValue(js, load);
        return Err(error::create("Game load must be function"));
    }

    auto update = JS_GetPropertyStr(js, ns, "update");
    if (!JS_IsUndefined(update) && !JS_IsNull(update) && !JS_IsFunction(js, update)) {
        JS_FreeValue(js, update);
        return Err(error::create("Game update must be function"));
    }

    auto draw = JS_GetPropertyStr(js, ns, "draw");
    if (!JS_IsUndefined(draw) && !JS_IsNull(draw) && !JS_IsFunction(js, draw)) {
        JS_FreeValue(js, draw);
        return Err(error::create("Game draw must be function"));
    }

    auto pre_reload = JS_GetPropertyStr(js, ns, "preReload");
    if (!JS_IsUndefined(pre_reload) && !JS_IsNull(pre_reload) && !JS_IsFunction(js, pre_reload)) {
        JS_FreeValue(js, pre_reload);
        return Err(error::create("Game preReload must be function"));
    }

    auto post_reload = JS_GetPropertyStr(js, ns, "postReload");
    if (!JS_IsUndefined(post_reload) && !JS_IsNull(post_reload) && !JS_IsFunction(js, post_reload)) {
        JS_FreeValue(js, post_reload);
        return Err(error::create("Game postReload must be function"));
    }

    return Game {
        .js = js,
        .config = *config,
        .mod = mod,
        .load = load,
        .update = update,
        .draw = draw,
        .pre_reload = pre_reload,
        .post_reload = post_reload,
    };
}

auto destroy(Game& self) -> void {
    JS_FreeValue(self.js, self.load);
    JS_FreeValue(self.js, self.update);
    JS_FreeValue(self.js, self.draw);
    JS_FreeValue(self.js, self.pre_reload);
    JS_FreeValue(self.js, self.post_reload);
}

auto safe_call(Game& self, JSValue func, std::span<JSValue> args = {}) -> Result<JSValue> {
    if (!JS_IsFunction(self.js, func)) {
        return JS_UNDEFINED;
    }

    auto length = args.size();
    auto argv = length == 0 ? nullptr : args.data();
    auto ret = JS_Call(self.js, func, JS_UNDEFINED, int(length), argv);
    if (JS_HasException(self.js)) {
        return Err(error::from_js(self.js, JS_GetException(self.js)));
    }

    return ret;
}

auto load(Game& self) -> Result<> {
    if (auto ret = safe_call(self, self.load)) {
        JS_FreeValue(self.js, *ret);
        return std::monostate {};
    } else {
        return Err(ret.error());
    }
}

auto update(Game& self) -> Result<> {
    if (auto ret = safe_call(self, self.update)) {
        JS_FreeValue(self.js, *ret);
        return std::monostate {};
    } else {
        return Err(ret.error());
    }
}

auto draw(Game& self) -> Result<> {
    if (auto ret = safe_call(self, self.draw)) {
        JS_FreeValue(self.js, *ret);
        return std::monostate {};
    } else {
        return Err(ret.error());
    }
}

auto pre_reload(Game& self) -> Result<JSValue> {
    return safe_call(self, self.pre_reload);
}

auto post_reload(Game& self, JSValue state) -> Result<> {
    auto args = std::array {state};

    if (auto ret = safe_call(self, self.post_reload, args)) {
        JS_FreeValue(self.js, *ret);
        return {};
    } else {
        return Err(ret.error());
    }
}

} // namespace muen::game

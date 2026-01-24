#include "engine.hpp"

#include <spdlog/spdlog.h>
#include <raylib.h>

#include <defer.hpp>
#include <engine/window.hpp>
#include <utility>

namespace muen {

extern "C" auto module_loader(JSContext *ctx, const char *module_name, void *opaque) noexcept -> JSModuleDef *;
auto read_config(js::Object& ns) -> Result<GameConfig>;

auto Engine::JSRuntime_deleter::operator()(JSRuntime *rt) noexcept -> void {
    if (rt == nullptr) return;
    JS_FreeRuntime(rt);
}

auto Engine::JSContext_deleter::operator()(JSContext *ctx) noexcept -> void {
    if (ctx == nullptr) return;
    JS_FreeContext(ctx);
}

auto Engine::create(const std::filesystem::path& base_path) noexcept -> Result<std::unique_ptr<Engine>> {
    window::setup();

    SPDLOG_TRACE("Creating JS runtime");
    auto runtime_ptr = JS_NewRuntime();
    if (runtime_ptr == nullptr) return err("Could not allocate runtime");
    auto runtime = std::unique_ptr<JSRuntime, JSRuntime_deleter>(runtime_ptr);

    SPDLOG_TRACE("Creating JS context");
    auto context_ptr = JS_NewContext(runtime.get());
    if (context_ptr == nullptr) return err("Could not allocate context");
    auto context = std::unique_ptr<JSContext, JSContext_deleter>(context_ptr);

    SPDLOG_TRACE("Creating file store");
    auto store = std::unique_ptr<IFileStore> {};
    if (std::filesystem::is_directory(base_path)) {
        if (auto r = FilesystemStore::open(base_path)) store = std::make_unique<FilesystemStore>(std::move(*r));
        else return err(r);
    } else {
        if (auto r = ZipStore::open(base_path)) store = std::make_unique<ZipStore>(std::move(*r));
        else return err(r);
    }

    SPDLOG_TRACE("Allocationg engine");
    auto engine_ptr = owner<Engine *>(new (std::nothrow) Engine {
        std::move(runtime),
        std::move(context),
        std::move(store),
    });
    if (engine_ptr == nullptr) return err("Could not allocate engine");
    auto engine = std::unique_ptr<Engine>(engine_ptr);

    JS_SetDumpFlags(engine->js_runtime(), JS_DUMP_LEAKS);
    JS_SetRuntimeOpaque(engine->js_runtime(), engine.get());
    JS_SetModuleLoaderFunc(engine->js_runtime(), nullptr, module_loader, engine.get());

    SPDLOG_TRACE("Engine created successfully");
    return engine;
}

auto Engine::get(not_null<JSRuntime *> rt) noexcept -> Engine& {
    return *static_cast<Engine *>(JS_GetRuntimeOpaque(rt.get()));
}

auto Engine::get(not_null<JSContext *> ctx) noexcept -> Engine& {
    return *static_cast<Engine *>(JS_GetRuntimeOpaque(JS_GetRuntime(ctx.get())));
}

auto Engine::js_runtime() const noexcept -> not_null<JSRuntime *> {
    return _js_runtime.get().get();
}

auto Engine::js_context() const noexcept -> not_null<JSContext *> {
    return _js_context.get().get();
}

auto Engine::file_store() noexcept -> IFileStore& {
    return *_file_store;
}

auto Engine::texture_store() noexcept -> ResourceStore<TextureData>& {
    return _texture_store;
}

auto Engine::font_store() noexcept -> ResourceStore<FontData>& {
    return _font_store;
}

auto Engine::register_plugin(const plugins::EnginePlugin& desc) noexcept -> void try {
    for (const auto& [name, module] : desc.c_modules) {
        // TODO: check if already exists
        _c_modules.insert({name, module});
    }

    for (const auto& [name, module] : desc.js_modules) {
        // TODO: check if already exists
        _js_modules.insert({name, module});
    }

    if (desc.load != nullptr) {
        _load_callbacks.emplace_back(desc.load);
    }

    if (desc.unload != nullptr) {
        _unload_callbacks.emplace_back(desc.unload);
    }

    if (desc.update != nullptr) {
        _update_callbacks.emplace_back(desc.update);
    }

    if (desc.draw != nullptr) {
        _draw_callbacks.emplace_back(desc.draw);
    }

    SPDLOG_INFO("Registered `{}` plugin", desc.name);
} catch (...) {
    std::terminate();
}

auto Engine::load_plugins() noexcept -> Result<> try {
    SPDLOG_DEBUG("Loading plugins");
    for (const auto& callback : _load_callbacks) {
        if (auto result = callback(); !result) return err(result);
    }
    return {};
} catch (std::exception& e) {
    return err(e);
}

[[nodiscard]] auto Engine::run_game(Game& game) noexcept -> Result<> try {
    defer({
        SPDLOG_TRACE("Unloading plugins");
        for (const auto& callback : _unload_callbacks) {
            if (auto result = callback(); !result) {
                SPDLOG_WARN("Error unloading plugin: {}", result.error()->msg());
            }
        }
    });

    SPDLOG_DEBUG("Creating window");
    auto w = window::create(
        window::Config {
            .width = game.config().window.width,
            .height = game.config().window.height,
            .fps = game.config().window.fps,
            .title = game.config().window.title,
            .window_flags = (game.config().window.vsync_hint ? FLAG_VSYNC_HINT : 0)
                | (game.config().window.fullscreen_mode ? FLAG_FULLSCREEN_MODE : 0)
                | (game.config().window.resizable ? FLAG_WINDOW_RESIZABLE : 0)
                | (game.config().window.undecorated ? FLAG_WINDOW_UNDECORATED : 0)
                | (game.config().window.hidden ? FLAG_WINDOW_HIDDEN : 0)
                | (game.config().window.minimized ? FLAG_WINDOW_MINIMIZED : 0)
                | (game.config().window.maximized ? FLAG_WINDOW_MAXIMIZED : 0)
                | (game.config().window.unfocused ? FLAG_WINDOW_UNFOCUSED : 0)
                | (game.config().window.topmost ? FLAG_WINDOW_TOPMOST : 0)
                | (game.config().window.always_run ? FLAG_WINDOW_ALWAYS_RUN : 0)
                | (game.config().window.transparent ? FLAG_WINDOW_TRANSPARENT : 0)
                | (game.config().window.highdpi ? FLAG_WINDOW_HIGHDPI : 0)
                | (game.config().window.mouse_passthrough ? FLAG_WINDOW_MOUSE_PASSTHROUGH : 0)
                | (game.config().window.borderless_windowed_mode ? FLAG_BORDERLESS_WINDOWED_MODE : 0)
                | (game.config().window.msaa_4x_hint ? FLAG_MSAA_4X_HINT : 0)
                | (game.config().window.interlaced_hint ? FLAG_INTERLACED_HINT : 0)
        }
    );
    defer({
        SPDLOG_TRACE("Closing window");
        window::close(w);
    });

    SPDLOG_DEBUG("Loading game");
    if (auto r = game.load(); !r) return err(r);

    SPDLOG_DEBUG("Running rame");
    while (!window::should_close(w)) {
        if (IsKeyPressed(KEY_F5)) {
            if (auto r = game.try_reload(); !r) {
                SPDLOG_ERROR("Exception occured while reloading the game: {}", r.error()->msg());
            }
        }

        SPDLOG_TRACE("Updating plugins");
        for (const auto& callback : _update_callbacks) {
            if (auto r = callback(); !r) return err(r);
        }

        SPDLOG_TRACE("Updating game");
        if (auto r = game.update(); !r) return err(r);

        window::begin_drawing(w);

        SPDLOG_TRACE("Drawing plugins");
        for (const auto& callback : _draw_callbacks) {
            if (auto r = callback(); !r) return err(r);
        }

        SPDLOG_TRACE("Drawing game");
        if (auto r = game.draw(); !r) return err(r);

        window::draw_fps(w);
        window::end_drawing(w);
    }

    return {};
} catch (std::exception& e) {
    return err(e);
}

auto Engine::load_module(const std::filesystem::path& name) noexcept -> Result<owner<JSModuleDef *>> try {
    if (auto cm = _c_modules.find(name); cm != _c_modules.end()) {
        SPDLOG_DEBUG("Module {} resolved as builtin native module", name.string());
        return cm->second;
    } else {
        auto code = std::string {};
        if (auto jm = _js_modules.find(name); jm != _js_modules.end()) {
            SPDLOG_DEBUG("Module {} resolved as builtin js module", name.string());
            code = jm->second;
        } else {
            auto path = name;
            // TODO: error handling
            if (!path.has_extension()) {
                path += ".js";
            }
            SPDLOG_TRACE("Loading module {}", path.string());
            const auto contents = _file_store->read_string(path);
            if (!contents) return err(contents);
            SPDLOG_DEBUG("Module {} resolved as game module", name.string());
            code = *contents;
        }

        JSValue ret = JS_Eval(
            js_context(),
            code.c_str(),
            code.size(),
            name.string().c_str(),
            JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY
        );

        if (JS_IsException(ret)) return nullptr;
        auto mod = static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(ret));
        JS_FreeValue(js_context(), ret);

        return mod;
    }
} catch (std::exception& e) {
    return err(e);
}

Engine::Engine(
    std::unique_ptr<JSRuntime, JSRuntime_deleter>&& runtime,
    std::unique_ptr<JSContext, JSContext_deleter>&& context,
    std::unique_ptr<IFileStore>&& store
) noexcept :
    _file_store {std::move(store)},
    _js_runtime {std::move(runtime)},
    _js_context {std::move(context)} {}

auto Game::create(not_null<JSContext *> js, not_null<IFileStore *> store) -> Result<Game> {
    SPDLOG_TRACE("Creating game");

    SPDLOG_TRACE("Reading game file");
    const auto src = store->read_string("game.js");
    if (!src) return err(src);

    SPDLOG_TRACE("Compiling game module");
    auto mod = JS_Eval(
        js,
        src->c_str(),
        src->size(),
        "game.js",
        JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT | JS_EVAL_FLAG_COMPILE_ONLY
    );
    if (JS_HasException(js)) return err(js::JSError::from_value(js::own(js, JS_GetException(js))));

    SPDLOG_TRACE("Evaluating game module");
    auto eval_ret = JS_EvalFunction(js, mod);
    defer(JS_FreeValue(js, eval_ret));
    auto eval_result = JS_PromiseResult(js, eval_ret);
    if (JS_IsError(eval_result)) return err(js::JSError::from_value(js::own(js, eval_result)));
    JS_FreeValue(js, eval_result);

    auto m = static_cast<JSModuleDef *>(JS_VALUE_GET_PTR(mod));
    auto ns_prop = JS_GetModuleNamespace(js, m);
    auto ns_val = js::own(js, ns_prop);
    auto ns = *js::Object::from_value(ns_val);

    SPDLOG_TRACE("Reading game config");
    auto config = read_config(ns);
    if (!config) return err(config);

    SPDLOG_TRACE("Reading game callbacks");
    auto load = ns.at<std::optional<js::Function>>("load");
    if (!load) return err("game.load is not a function");
    auto update = ns.at<std::optional<js::Function>>("update");
    if (!update) return err("game.update is not a function");
    auto draw = ns.at<std::optional<js::Function>>("draw");
    if (!draw) return err("game.draw is not a function");
    auto pre_reload = ns.at<std::optional<js::Function>>("preReload");
    if (!pre_reload) return err("game.preReload is not a function");
    auto post_reload = ns.at<std::optional<js::Function>>("postReload");
    if (!post_reload) return err("game.postReload is not a function");

    SPDLOG_TRACE("Game created successfully");
    return Game({
        .js = js,
        .store = store,
        .config = *config,
        .load = std::move(*load),
        .update = std::move(*update),
        .draw = std::move(*draw),
        .pre_reload = std::move(*pre_reload),
        .post_reload = std::move(*post_reload),
    });
}

[[nodiscard]]
auto Game::config() const noexcept -> const GameConfig& {
    return _config;
}

[[nodiscard]]
auto Game::load() noexcept -> Result<> {
    if (!_load) return {};
    if (auto r = _load->operator()(); !r) return err(r);
    return {};
}

[[nodiscard]]
auto Game::update() noexcept -> Result<> {
    if (!_update) return {};
    if (auto r = _update.value()(); !r) return err(r);
    return {};
}

[[nodiscard]]
auto Game::draw() noexcept -> Result<> {
    if (!_draw) return {};
    if (auto r = _draw->operator()(); !r) return err(r);
    return {};
}

[[nodiscard]]
auto Game::try_reload() noexcept -> Result<> try {
    std::optional<js::Value> state {};
    SPDLOG_TRACE("Reloading game");

    if (_pre_reload) {
        SPDLOG_TRACE("Game has preReload, so calling it");
        auto r = _pre_reload->operator()();
        if (!r) return err(r);
        state = std::move(*r);
    }

    SPDLOG_TRACE("Recreating game");
    auto game_result = Game::create(_js, _store);
    if (!game_result) return err(game_result);
    auto new_game = std::move(*game_result);

    if (new_game._post_reload) {
        SPDLOG_TRACE("New game has postReload, so calling it");
        if (state) {
            auto args = std::array {*state};
            if (auto r = new_game._post_reload->operator()(args); !r) return err(r);
        } else {
            if (auto r = new_game._post_reload->operator()(); !r) return err(r);
        }
    }

    SPDLOG_TRACE("Successfully reloaded game");
    *this = std::move(new_game);
    return {};
} catch (std::exception& e) {
    return err(e);
}

Game::Game(InitParams p) noexcept :
    _js(p.js),
    _store(p.store),
    _config(std::move(p.config)),
    _load(std::move(p.load)),
    _update(std::move(p.update)),
    _draw(std::move(p.draw)),
    _pre_reload(std::move(p.pre_reload)),
    _post_reload(std::move(p.post_reload)) {}

extern "C" auto module_loader(JSContext *ctx, const char *module_name, void *opaque) noexcept -> JSModuleDef * {
    SPDLOG_DEBUG("Loading module {}", module_name);
    auto e = static_cast<Engine *>(opaque);
    if (auto mod = e->load_module(module_name)) {
        return *mod;
    } else {
        JS_ThrowPlainError(ctx, "%s", std::format("{}", mod.error()->msg()).c_str());
        return nullptr;
    }
}

#define MUEN_GAMECONFIG_READ_OPTIONAL(object, variable, jsname)                                                        \
    do { /* NOLINT */                                                                                                  \
        if (auto v = (object).at<std::optional<decltype(variable)>>(#jsname)) {                                        \
            if (*v) (variable) = **v;                                                                                  \
        } else {                                                                                                       \
            return err(v);                                                                                             \
        }                                                                                                              \
    } while (0)

auto read_config(js::Object& ns) -> Result<GameConfig> {
    GameConfig config {};

    auto obj_result = ns.at<std::optional<js::Object>>("config");
    if (!obj_result) return err(obj_result);
    auto obj_opt = std::move(*obj_result);
    if (!obj_opt) return config;
    auto obj = std::move(*obj_opt);
    auto window_obj_result = obj.at<std::optional<js::Object>>("window");
    if (!window_obj_result) return err(window_obj_result);
    if (!window_obj_result->has_value()) return config;
    auto window_obj = std::move(**window_obj_result);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.title, title);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.width, width);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.height, height);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.fps, fps);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.vsync_hint, vsync);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.fullscreen_mode, fullscreenMode);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.resizable, resizable);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.undecorated, undecorated);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.hidden, hidden);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.minimized, minimized);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.maximized, maximized);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.unfocused, unfocused);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.topmost, topmost);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.always_run, alwaysRun);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.transparent, transparent);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.highdpi, highdpi);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.mouse_passthrough, mousePassthrough);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.borderless_windowed_mode, borderlessWindowedMode);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.msaa_4x_hint, msaa4x);
    MUEN_GAMECONFIG_READ_OPTIONAL(window_obj, config.window.interlaced_hint, interlaced);

    return config;
}

} // namespace muen

#include "./engine/audio.cpp"
#include "./engine/music.cpp"
#include "./engine/sound.cpp"
#include "./engine/window.cpp"

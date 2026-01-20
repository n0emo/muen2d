#pragma once

#include <string>
#include <unordered_map>

#include <gsl/gsl>
#include <spdlog/spdlog.h>

#include <quickjs.hpp>
#include <types.hpp>
#include <engine/plugin.hpp>
#include <error.hpp>
#include <file_store.hpp>

namespace muen {

using namespace gsl;

class Game;
class Engine;

class Engine {
  private:
    struct JSRuntime_deleter {
        auto operator()(JSRuntime *rt) noexcept -> void;
    };

    struct JSContext_deleter {
        auto operator()(JSContext *ctx) noexcept -> void;
    };

  private:
    not_null<std::unique_ptr<JSRuntime, JSRuntime_deleter>> _js_runtime;
    not_null<std::unique_ptr<JSContext, JSContext_deleter>> _js_context;
    not_null<std::unique_ptr<IFileStore>> _store;
    std::unordered_map<std::filesystem::path, std::string> _js_modules {};
    std::unordered_map<std::filesystem::path, JSModuleDef *> _c_modules {};
    std::vector<std::function<auto()->Result<>>> _load_callbacks {};
    std::vector<std::function<auto()->Result<>>> _unload_callbacks {};
    std::vector<std::function<auto()->Result<>>> _update_callbacks {};
    std::vector<std::function<auto()->Result<>>> _draw_callbacks {};

  public:
    [[nodiscard]]
    static auto create(const std::filesystem::path& base_path) noexcept -> Result<std::unique_ptr<Engine>>;

    [[nodiscard]]
    static auto get(not_null<JSRuntime *> rt) noexcept -> Engine&;

    [[nodiscard]]
    static auto get(not_null<JSContext *> ctx) noexcept -> Engine&;

    [[nodiscard]]
    auto js_runtime() const noexcept -> not_null<JSRuntime *>;

    [[nodiscard]]
    auto js_context() const noexcept -> not_null<JSContext *>;

    [[nodiscard]]
    auto store() const noexcept -> IFileStore&;

    /// Register plugin to engine
    auto register_plugin(const plugins::EnginePlugin& desc) noexcept -> void;

    /// Call this after all plugins registered
    auto load_plugins() noexcept -> Result<>;

    /// Run game using engine
    [[nodiscard]]
    auto run_game(Game& game) noexcept -> Result<>;

    [[nodiscard]]
    auto load_module(const std::filesystem::path& path) noexcept -> Result<owner<JSModuleDef *>>;

  private:
    Engine(
        std::unique_ptr<JSRuntime, JSRuntime_deleter>&& runtime,
        std::unique_ptr<JSContext, JSContext_deleter>&& context,
        std::unique_ptr<IFileStore>&& store
    ) noexcept;
};

struct GameConfig {
    int width = 800;
    int height = 600;
    int fps = 60;
    std::string title = "muen2d";
};

class Game {
  private:
    not_null<JSContext *> _js;
    not_null<IFileStore *> _store;
    GameConfig _config;
    std::optional<js::Function> _load;
    std::optional<js::Function> _update;
    std::optional<js::Function> _draw;
    std::optional<js::Function> _pre_reload;
    std::optional<js::Function> _post_reload;

  public:
    static auto create(not_null<JSContext *> js, not_null<IFileStore *> store) -> Result<Game>;

    [[nodiscard]]
    auto config() const noexcept -> const GameConfig&;

    [[nodiscard]]
    auto load() noexcept -> Result<>;

    [[nodiscard]]
    auto update() noexcept -> Result<>;

    [[nodiscard]]
    auto draw() noexcept -> Result<>;

    [[nodiscard]]
    auto try_reload() noexcept -> Result<>;

  private:
    struct InitParams {
        not_null<JSContext *> js;
        not_null<IFileStore *> store;
        GameConfig config;
        std::optional<js::Function> load;
        std::optional<js::Function> update;
        std::optional<js::Function> draw;
        std::optional<js::Function> pre_reload;
        std::optional<js::Function> post_reload;
    };

    Game(InitParams p) noexcept;
};

[[nodiscard]] auto load_texture(not_null<JSRuntime *> js, const std::filesystem::path& path) noexcept
    -> Result<Texture>;

[[nodiscard]] auto load_sound(not_null<JSRuntime *> js, const std::filesystem::path& path) noexcept -> Result<Sound>;

[[nodiscard]] auto load_music(not_null<JSRuntime *> js, const std::filesystem::path& path) noexcept -> Result<Music>;

} // namespace muen

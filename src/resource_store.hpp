#pragma once

#include <atomic>
#include <cstdint>
#include <gsl/gsl>
#include <memory>
#include <string>
#include <unordered_map>

#include <spdlog/spdlog.h>

#include <file_store.hpp>

namespace muen {

using namespace gsl;

template<typename T>
concept is_data_v = requires(T d, T::data_type v) {
    typename T::data_type;
    { T {} } -> std::same_as<T>;
    { d.get() } -> std::same_as<typename T::data_type&>;
};

template<typename T>
    requires is_data_v<T>
struct Resource {
    T data {};
    std::atomic_int32_t ref_count {};
};

template<typename T>
    requires is_data_v<T>
class ResourceStore {
  public:
    using Handle = int32_t;

  private:
    std::unordered_map<Handle, std::unique_ptr<Resource<T>>> _table {};
    std::unordered_map<std::string, Handle> _cache {};
    T _default {};

  public:
    auto load(std::string name, const std::function<auto()->T>& load_callback) noexcept -> Handle try {
        SPDLOG_TRACE("Loading Resource with name {}", name);

        if (auto it = _cache.find(name); it != _cache.end()) {
            SPDLOG_TRACE("Found resource {} in cache", name);
            get(it->second);
            return it->second;
        }

        auto data = load_callback();
        auto res = std::make_unique<Resource<T>>(std::move(data), 1);
        auto handle = next_handle();
        SPDLOG_DEBUG("Loaded resource [{}]", handle);
        _table.insert({handle, std::move(res)});
        _cache.insert({name, handle});
        return handle;
    } catch (std::exception& e) {
        SPDLOG_WARN("Unexpected C++ exception while loading resource: {}", e.what());
        return 0;
    }

    auto load_by_name(const std::string& name) noexcept -> Handle {
        auto handle_search = _cache.find(name);
        if (handle_search == _cache.end()) return 0;
        const auto handle = handle_search->second;
        auto data_search = _table.find(handle);
        if (data_search == _table.end()) {
            SPDLOG_ERROR("Handle is prese");
            return 0;
        };
        auto ptr = data_search->second.get();
        ptr->ref_count++;
        return handle;
    }

    auto get(Handle handle) noexcept -> const T::data_type& {
        if (auto it = _table.find(handle); it != _table.end()) {
            auto ptr = it->second.get();
            ptr->ref_count++;
            return ptr->data.get();
        } else {
            return _default.get();
        }
    }

    auto get_by_name(const std::string& name) noexcept -> const T::data_type& {
        auto handle_search = _cache.find(name);
        if (handle_search == _cache.end()) return _default.get();
        const auto handle = handle_search->second;
        auto data_search = _table.find(handle);
        if (data_search == _table.end()) return _default.get();
        return data_search->second.get();
    }

    auto borrow(Handle handle) noexcept -> const T::data_type& {
        if (auto it = _table.find(handle); it != _table.end()) {
            return it->second->data.get();
        } else {
            return _default.get();
        }
    }

    auto release(Handle handle) noexcept -> void {
        if (auto it = _table.find(handle); it != _table.end()) {
            auto ptr = it->second.get();
            ptr->ref_count--;
            if (ptr->ref_count <= 0) {
                if (ptr->ref_count < 0) SPDLOG_WARN("Reference count of resource is < 0");
                SPDLOG_DEBUG("Unloading resource [{}]", handle);
                _table.erase(handle);
                std::erase_if(_cache, [=](const auto& item) -> bool { return item.second == handle; });
            }
        }
    }

    auto clear() noexcept -> void {
        _table.clear();
        _cache.clear();
    }

  private:
    static auto next_handle() noexcept -> Handle {
        static std::atomic_int32_t _handle_counter = 1;
        return _handle_counter++;
    }
};

} // namespace muen

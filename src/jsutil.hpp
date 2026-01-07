#pragma once

#include <expected>
#include <string>
#include <type_traits>
#include <optional>
#include <vector>

#include <spdlog/spdlog.h>
#include <quickjs.h>

#include <defer.hpp>

namespace js {

static inline constexpr auto display_type(int64_t type) -> const char * {
    switch (type) {
        case JS_TAG_BIG_INT:
            return "BigInt";
        case JS_TAG_SYMBOL:
            return "Symbol";
        case JS_TAG_STRING:
            return "String";
        case JS_TAG_MODULE:
            return "Module";
        case JS_TAG_FUNCTION_BYTECODE:
            return "FunctionBytecode";
        case JS_TAG_OBJECT:
            return "Object";
        case JS_TAG_INT:
            return "Int";
        case JS_TAG_BOOL:
            return "Bool";
        case JS_TAG_NULL:
            return "Null";
        case JS_TAG_UNDEFINED:
            return "Undefined";
        case JS_TAG_UNINITIALIZED:
            return "Uninitialized";
        case JS_TAG_CATCH_OFFSET:
            return "CatchOffset";
        case JS_TAG_EXCEPTION:
            return "Exception";
        case JS_TAG_SHORT_BIG_INT:
            return "ShortBigInt";
        case JS_TAG_FLOAT64:
            return "Float64";
        default:
            return "Unknown";
    }
}

template<typename T>
concept is_container = requires(T a, typename T::value_type v) {
    typename T::value_type;
    { T {} } -> std::same_as<T>;
    { a.push_back(v) } -> std::same_as<void>;
};

static_assert(is_container<std::vector<int>>);

template<typename T>
concept is_optional = requires(T a, typename T::value_type v) {
    typename T::value_type;
    requires std::is_constructible_v<T, std::nullopt_t>;
    requires std::is_constructible_v<T, typename T::value_type>;
};

static_assert(is_optional<std::optional<int>>);
static_assert(!is_optional<std::string>);

auto to_string(::JSContext *js, ::JSValueConst value) -> std::string;

template<typename T>
auto try_as(::JSContext *js, ::JSValueConst value) -> std::expected<T, ::JSValue>;

template<typename T>
auto try_into(::JSContext *js, JSValueConst value) -> std::expected<T, ::JSValue>;

template<typename T>
auto try_get_property(::JSContext *js, ::JSValueConst value, const std::string& name) -> std::expected<T, ::JSValue>;

template<typename T>
auto try_as(::JSContext *js, ::JSValueConst value) -> std::expected<T, ::JSValue> {
    if constexpr (std::is_same_v<T, ::JSValue>) {
        SPDLOG_TRACE("Converting JS value to itself (no-op)");
        return value;

    } else if constexpr (std::is_same_v<T, double>) {
        SPDLOG_TRACE("Converting JS value to double");
        if (!::JS_IsNumber(value)) {
            return std::unexpected(
                ::JS_NewTypeError(js, "Value of type '%s' is not a number", display_type(value.tag))
            );
        }

        auto num = double {};
        ::JS_ToFloat64(js, &num, value);

        return num;

    } else if constexpr (std::is_same_v<T, bool>) {
        SPDLOG_TRACE("Converting JS value to bool");
        if (!::JS_IsBool(value)) {
            return std::unexpected(
                ::JS_NewTypeError(js, "Value of type '%s' is not a boolean", display_type(value.tag))
            );
        }

        return ::JS_ToBool(js, value);

    } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        SPDLOG_TRACE("Converting JS value to numeric");
        auto num = try_as<double>(js, value);
        if (!num)
            return std::unexpected(num.error());
        return static_cast<T>(*num);

    } else if constexpr (std::is_same_v<T, std::string>) {
        SPDLOG_TRACE("Converting JS value to string");
        if (!::JS_IsString(value)) {
            return std::unexpected(
                ::JS_NewTypeError(js, "Value of type '%s' is not a string", display_type(value.tag))
            );
        }

        auto len = size_t {};
        const auto cstr = ::JS_ToCStringLen(js, &len, value);
        defer(JS_FreeCString(js, cstr));
        return std::string {cstr, len};

    } else if constexpr (is_container<T>) {
        SPDLOG_TRACE("Converting JS value to container");
        auto length = int64_t {};
        ::JS_GetLength(js, value, &length);

        auto container = T {};
        if (requires { container.reserve(std::declval<size_t>()); }) {
            container.reserve(static_cast<size_t>(length));
        }

        for (auto i = 0; i < length; i++) {
            const auto prop = ::JS_GetPropertyInt64(js, value, i);
            const auto val = try_into<typename T::value_type>(js, prop);
            if (!val.has_value()) {
                return std::unexpected(val.error());
            }
            container.push_back(*val);
        }
        return container;

    } else if constexpr (is_optional<T>) {
        SPDLOG_TRACE("Converting JS value to optional");
        if (::JS_IsUndefined(value) || ::JS_IsUninitialized(value) || ::JS_IsNull(value)) {
            return std::nullopt;
        } else {
            return try_as<typename T::value_type>(js, value);
        }

    } else {
        static_assert(false, "Could not convert JSValue to specified type");
    }
}

template<typename T>
auto try_into(::JSContext *js, JSValueConst value) -> std::expected<T, ::JSValue> {
    defer(JS_FreeValue(js, value));
    return try_as<T>(js, value);
}

template<>
auto try_into<::JSValue>(::JSContext *js, JSValueConst value) -> std::expected<::JSValue, ::JSValue>;

template<typename T>
auto try_get_property(::JSContext *js, ::JSValueConst value, const std::string& name) -> std::expected<T, ::JSValue> {
    SPDLOG_TRACE("Getting property `{}` from object", name);
    if (!::JS_IsObject(value)) {
        return std::unexpected(::JS_NewTypeError(js, "Value of type '%s' is not an object", display_type(value.tag)));
    }

    auto atom = ::JS_NewAtom(js, name.c_str());
    if (atom == JS_ATOM_NULL) {
        return std::unexpected(::JS_NewInternalError(js, "Could not create a new atom"));
    }
    defer(::JS_FreeAtom(js, atom));

    if (!::JS_HasProperty(js, value, atom)) {
        if constexpr (is_optional<T>) {
            return std::nullopt;
        } else {
            return std::unexpected(::JS_NewReferenceError(js, "Property `%s` does not exist on object", name.c_str()));
        }
    }

    SPDLOG_TRACE("Property `{}` exists, converting to target type", name);
    const auto prop = ::JS_GetProperty(js, value, atom);
    return try_into<T>(js, prop);
}

template<typename T>
concept TryAsCompiles = requires(::JSContext *js, ::JSValueConst value) { try_as<T>(js, value); };

} // namespace js

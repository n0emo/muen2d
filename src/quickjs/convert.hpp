#pragma once

#include <tuple>

#include <fmt/format.h>

#include <quickjs/error.hpp>
#include <quickjs/function.hpp>
#include <quickjs/utils.hpp>
#include <quickjs/value.hpp>
#include <quickjs/object.hpp>

namespace glint::js {

template<typename T>
auto convert_from_js(const Value& v) noexcept -> JSResult<T>;

template<typename T>
auto convert_to_js(JSContext *ctx, T val) noexcept -> JSResult<Value>;

template<typename... Ts>
auto unpack_args(JSContext *js, int argc, JSValueConst *argv) -> JSResult<std::tuple<Ts...>>;

template<typename Tuple>
auto unpack_args_t(JSContext *js, int args, JSValueConst *argv) -> JSResult<Tuple>;

// ----------------
//  Implementation
// ----------------

template<>
inline auto convert_from_js<JSValue>(const Value& v) noexcept -> JSResult<JSValue> {
    return v.cget();
}

template<>
inline auto convert_from_js<Value>(const Value& v) noexcept -> JSResult<Value> {
    return v;
}

template<>
inline auto convert_from_js<bool>(const Value& v) noexcept -> JSResult<bool> {
    if (!JS_IsBool(v.cget())) {
        return Unexpected(
            JSError::type_error(v.ctx(), fmt::format("Value of type '{}' is not a boolean", display_type(v)))
        );
    }

    return JS_ToBool(v.ctx(), v.cget());
}

template<>
inline auto convert_from_js<double>(const Value& v) noexcept -> JSResult<double> {
    if (!JS_IsNumber(v.cget())) {
        return Unexpected(
            JSError::type_error(v.ctx(), fmt::format("Value of type '{}' is not a number", display_type(v)))
        );
    }

    auto num = double {};
    JS_ToFloat64(v.ctx(), &num, v.cget());

    return num;
}

template<>
inline auto convert_from_js<std::string>(const Value& v) noexcept -> JSResult<std::string> {
    if (!JS_IsString(v.cget())) {
        return Unexpected(
            JSError::type_error(v.ctx(), fmt::format("Value of type '{}' is not a string", display_type(v)))
        );
    }

    auto len = size_t {};
    const auto cstr = JS_ToCStringLen(v.ctx(), &len, v.cget());
    defer(JS_FreeCString(v.ctx(), cstr));
    return std::string {cstr, len};
}

template<>
inline auto convert_from_js<Object>(const Value& v) noexcept -> JSResult<Object> {
    return Object::from_value(v);
}

template<>
inline auto convert_from_js<Function>(const Value& v) noexcept -> JSResult<Function> {
    return Function::from_value(v);
}

template<typename T>
    requires(std::is_integral_v<T> || std::is_floating_point_v<T>) && (!std::is_same_v<T, double>)
    && (!std::is_same_v<T, bool>)
inline auto convert_from_js(const Value& v) noexcept -> JSResult<T> {
    auto num = convert_from_js<double>(v);
    if (!num) return Unexpected(num.error());
    return static_cast<T>(*num);
}

template<typename T>
    requires is_container<T> && (!is_basic_string_v<T>)
inline auto convert_from_js(const Value& v) noexcept -> JSResult<T> try {
    if (!JS_IsArray(v.cget())) {
        return Unexpected(
            JSError::type_error(v.ctx(), fmt::format("Value of type `{}` is not an Array", display_type(v)))
        );
    }

    auto length = int64_t {};
    JS_GetLength(v.ctx(), v.cget(), &length);

    auto container = T {};
    if (requires { container.reserve(std::declval<size_t>()); }) {
        container.reserve(static_cast<size_t>(length));
    }

    for (auto i = 0; i < length; i++) {
        const auto prop = own(v.ctx(), JS_GetPropertyInt64(v.ctx(), v.cget(), i));
        const auto val = convert_from_js<typename T::value_type>(prop);
        if (!val) return Unexpected(val.error());
        container.push_back(*val);
    }
    return container;
} catch (std::exception& e) {
    return Unexpected(js::JSError::plain_error(v.ctx(), fmt::format("Unexpected error: {}", e.what())));
}

template<typename T>
    requires is_optional<T>
inline auto convert_from_js(const Value& v) noexcept -> JSResult<T> {
    if (JS_IsUndefined(v.cget()) || JS_IsNull(v.cget())) {
        return std::nullopt;
    } else {
        return convert_from_js<typename T::value_type>(v).transform([](auto&& arg) -> auto {
            return std::optional(arg);
        });
    }
}

template<>
inline auto convert_to_js<JSValue>(JSContext *ctx, JSValue val) noexcept -> JSResult<Value> {
    return borrow(ctx, val);
}

template<>
inline auto convert_to_js<Value>(JSContext *ctx, Value val) noexcept -> JSResult<Value> {
    (void)ctx;
    return val;
}

template<>
inline auto convert_to_js<bool>(JSContext *ctx, bool val) noexcept -> JSResult<Value> {
    return own(ctx, JS_NewBool(ctx, val));
}

template<>
inline auto convert_to_js<double>(JSContext *ctx, double val) noexcept -> JSResult<Value> {
    return own(ctx, JS_NewFloat64(ctx, val));
}

template<>
// NOLINTNEXTLINE
inline auto convert_to_js<std::string>(JSContext *ctx, std::string val) noexcept -> JSResult<Value> {
    auto jsval = JS_NewStringLen(ctx, val.c_str(), val.size());
    return own(ctx, jsval);
}

template<typename T>
    requires(std::is_integral_v<T> || std::is_floating_point_v<T>) && (!std::is_same_v<T, double>)
    && (!std::is_same_v<T, bool>)
inline auto convert_to_js(JSContext *ctx, T val) noexcept -> JSResult<Value> {
    auto num = static_cast<double>(val);
    return own(ctx, JS_NewFloat64(ctx, num));
}

template<typename... Ts>
auto unpack_args(JSContext *js, int argc, JSValueConst *argv) -> JSResult<std::tuple<Ts...>> {
    return unpack_args_t<std::tuple<Ts...>>(js, argc, argv);
}

template<typename Tuple>
auto unpack_args_t(JSContext *js, int argc, JSValueConst *argv) -> JSResult<Tuple> {
    constexpr size_t N = std::tuple_size<Tuple>();

    if (argc < int {N}) {
        return Unexpected(JSError::range_error(js, fmt::format("Expected {} arguments, got {}", N, argc)));
    }

    return [&]<size_t... Is>(std::index_sequence<Is...>) -> auto {
        auto try_all = [&]() -> JSResult<Tuple> {
            auto args =
                std::make_tuple(convert_from_js<std::tuple_element_t<Is, Tuple>>(Value::borrowed(js, argv[Is]))...);

            if ((... || !std::get<Is>(args).has_value())) {
                std::optional<JSError> first_error;

                (void)((std::get<Is>(args).has_value() ? false : (first_error = std::get<Is>(args).error(), true))
                       || ...);

                return Unexpected(*first_error);
            }
            return std::make_tuple(std::move(*std::get<Is>(args))...);
        };

        return try_all();
    }(std::make_index_sequence<N> {});
}

template<>
inline auto unpack_args_t<std::tuple<>>(JSContext *, int, JSValueConst *) -> JSResult<std::tuple<>> {
    return std::tuple<>();
}

} // namespace glint::js

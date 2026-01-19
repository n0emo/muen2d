#pragma once

#include <expected>
#include <optional>
#include <span>
#include <string>
#include <tuple>
#include <type_traits>
#include <source_location>

#include <spdlog/spdlog.h>
#include <quickjs.h>
#include <gsl/gsl>

#include <error.hpp>
#include <defer.hpp>

namespace muen::js {

using namespace gsl;

template<class T>
struct is_basic_string: std::false_type {};

template<class C, class T, class A>
struct is_basic_string<std::basic_string<C, T, A>>: std::true_type {};

template<class T>
constexpr bool is_basic_string_v = is_basic_string<T>::value;

template<typename T>
concept is_container = requires(T a, typename T::value_type v) {
    typename T::value_type;
    { T {} } -> std::same_as<T>;
    { a.push_back(v) } -> std::same_as<void>;
};

template<typename T>
concept is_optional = requires(T a, typename T::value_type v) {
    typename T::value_type;
    requires std::is_constructible_v<T, std::nullopt_t>;
    requires std::is_constructible_v<T, typename T::value_type>;
};

class Value;
class Function;
class Object;
class JSError;

template<typename T = std::monostate>
using JSResult = Result<T, JSError>;

class Value {
  private:
    class Owned {
        friend class Value;

      private:
        JSContext *_ctx;
        std::optional<JSValue> _value {};

      public:
        Owned(not_null<JSContext *> js, JSValue value) noexcept;
        ~Owned() noexcept;
        Owned(const Owned&) noexcept;
        Owned(Owned&&) noexcept;
        auto operator=(const Owned&) noexcept -> Owned&;
        auto operator=(Owned&&) noexcept -> Owned&;

      public:
        friend auto swap(Owned& a, Owned& b) noexcept -> void;
    };

    class Borrowed {
        friend class Value;

      private:
        JSContext *_ctx;
        JSValue _value {};

      public:
        Borrowed(not_null<JSContext *> ctx, JSValue value) noexcept;
    };

  private:
    std::variant<Owned, Borrowed> _v;

  public:
    static auto owned(not_null<JSContext *> ctx, JSValue val) noexcept -> Value;
    static auto borrowed(not_null<JSContext *> ctx, JSValue val) noexcept -> Value;

    [[nodiscard]]
    auto ctx() const noexcept -> not_null<JSContext *>;

    [[nodiscard]]
    auto get() noexcept -> JSValue&;

    [[nodiscard]]
    auto cget() const noexcept -> const JSValue&;

  private:
    Value(Owned&& value) noexcept;
    Value(Borrowed value) noexcept;

  public:
    friend auto swap(Owned& a, Owned& b) noexcept -> void;
};

class Function {
  private:
    Value _value;

  public:
    static auto from_value(Value value) noexcept -> JSResult<Function>;

    auto operator()() noexcept -> JSResult<Value>;
    auto operator()(std::span<Value> args) noexcept -> JSResult<Value>;
    auto operator()(JSValueConst this_value, std::span<Value> args) noexcept -> JSResult<Value>;

  private:
    Function(Value&& value) noexcept;
};

class Object {
  private:
    Value _value;

  public:
    static auto from_value(Value value) noexcept -> JSResult<Object>;

    [[nodiscard]]
    auto get() noexcept -> Value&;

    [[nodiscard]]
    auto cget() const noexcept -> const Value&;

    template<typename T>
    [[nodiscard]]
    auto at(czstring prop) const noexcept -> JSResult<T>;

    template<typename T>
    [[nodiscard]]
    auto at(const std::string& prop) const noexcept -> JSResult<T>;

  private:
    Object(Value&& value) noexcept;
};

class JSError: public muen::error::IError {
  private:
    Object _obj;
    std::source_location _loc;

  public:
    explicit JSError(Object&& obj, std::source_location loc = std::source_location::current()) noexcept;
    ~JSError() override = default;
    JSError(const JSError&) = default;
    JSError(JSError&&) = default;
    auto operator=(const JSError&) -> JSError& = default;
    auto operator=(JSError&&) -> JSError& = default;

    static auto from_value(const Value& val, std::source_location loc = std::source_location::current()) noexcept
        -> JSError;

    static auto plain_error(
        not_null<JSContext *> ctx,
        const std::string& msg,
        std::source_location loc = std::source_location::current()
    ) noexcept -> JSError;

    static auto type_error(
        not_null<JSContext *> ctx,
        const std::string& msg,
        std::source_location loc = std::source_location::current()
    ) noexcept -> JSError;

    static auto range_error(
        not_null<JSContext *> ctx,
        const std::string& msg,
        std::source_location loc = std::source_location::current()
    ) noexcept -> JSError;

    [[nodiscard]]
    auto msg() const noexcept -> std::string override;

    [[nodiscard]]
    auto loc() const noexcept -> std::optional<std::source_location> override;

    [[nodiscard]]
    auto get() noexcept -> Object&;

    [[nodiscard]]
    auto cget() const noexcept -> const Object&;

    [[nodiscard]]
    auto value() noexcept -> JSValue&;

    [[nodiscard]]
    auto cvalue() const noexcept -> const JSValue&;

    [[nodiscard]]
    auto message() const noexcept -> std::optional<std::string>;

    [[nodiscard]]
    auto stack() const noexcept -> std::optional<std::string>;
};

auto borrow(not_null<JSContext *> ctx, JSValue val) noexcept -> Value;

auto own(not_null<JSContext *> ctx, JSValue val) noexcept -> Value;

auto jsthrow(JSError e) noexcept -> JSValue;

template<typename T>
inline auto try_into(const Value& v) noexcept -> JSResult<T>;

template<typename... Ts>
auto unpack_args(JSContext *js, int argc, JSValueConst *argv) -> JSResult<std::tuple<Ts...>>;

template<auto T>
auto class_id(not_null<JSContext *> js) -> JSClassID;

template<auto T>
auto class_id(not_null<JSRuntime *> rt) -> JSClassID;

auto to_string(JSContext *js, JSValueConst value) -> std::string;

auto display_type(int64_t tag) -> czstring;

auto display_type(JSValueConst val) -> czstring;

auto display_type(const Value& val) -> czstring;

} // namespace muen::js

namespace muen {

[[nodiscard]]
auto err(js::JSError e) noexcept -> Unexpected<Error>;

template<typename T>
[[nodiscard]]
auto err(const js::JSResult<T>& r) noexcept -> Unexpected<Error>;

} // namespace muen

// --------------
// Implementation
// --------------

namespace muen::js {

template<typename T>
auto Object::at(czstring name) const noexcept -> JSResult<T> {
    auto prop = Value::owned(_value.ctx(), JS_GetPropertyStr(_value.ctx(), _value.cget(), name));
    return try_into<T>(prop);
}

template<typename T>
auto Object::at(const std::string& name) const noexcept -> JSResult<T> {
    return at<T>(name.c_str());
}

template<auto T>
auto class_id(not_null<JSContext *> js) -> JSClassID {
    return class_id<T>(JS_GetRuntime(js));
}

template<auto T>
auto class_id(not_null<JSRuntime *> rt) -> JSClassID {
    static auto id = JSClassID {0};
    JS_NewClassID(rt, &id);
    return id;
}

template<>
inline auto try_into<JSValue>(const Value& v) noexcept -> JSResult<JSValue> {
    return v.cget();
}

template<>
inline auto try_into<Value>(const Value& v) noexcept -> JSResult<Value> {
    return v;
}

template<>
inline auto try_into<bool>(const Value& v) noexcept -> JSResult<bool> {
    if (!JS_IsBool(v.cget())) {
        return Unexpected(
            JSError::type_error(v.ctx(), fmt::format("Value of type '{}' is not a boolean", display_type(v)))
        );
    }

    return JS_ToBool(v.ctx(), v.cget());
    return false;
}

template<>
inline auto try_into<double>(const Value& v) noexcept -> JSResult<double> {
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
inline auto try_into<std::string>(const Value& v) noexcept -> JSResult<std::string> {
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
inline auto try_into<Object>(const Value& v) noexcept -> JSResult<Object> {
    return Object::from_value(v);
}

template<>
inline auto try_into<Function>(const Value& v) noexcept -> JSResult<Function> {
    return Function::from_value(v);
}

template<typename T>
    requires(std::is_integral_v<T> || std::is_floating_point_v<T>) && (!std::is_same_v<T, double>)
    && (!std::is_same_v<T, bool>)
inline auto try_into(const Value& v) noexcept -> JSResult<T> {
    auto num = try_into<double>(v);
    if (!num) return Unexpected(num.error());
    return static_cast<T>(*num);
}

template<typename T>
    requires is_container<T> && (!is_basic_string_v<T>)
inline auto try_into(const Value& v) noexcept -> JSResult<T> try {
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
        const auto val = try_into<typename T::value_type>(prop);
        if (!val) return Unexpected(val.error());
        container.push_back(*val);
    }
    return container;
} catch (std::exception& e) {
    return Unexpected(js::JSError::plain_error(v.ctx(), fmt::format("Unexpected error: {}", e.what())));
}

template<typename T>
    requires is_optional<T>
inline auto try_into(const Value& v) noexcept -> JSResult<T> {
    if (JS_IsUndefined(v.cget()) || JS_IsNull(v.cget())) {
        return std::nullopt;
    } else {
        return try_into<typename T::value_type>(v);
    }
}

template<typename... Ts>
auto unpack_args(JSContext *js, int argc, JSValueConst *argv) -> JSResult<std::tuple<Ts...>> {
    constexpr size_t N = sizeof...(Ts);

    if (argc < int {N}) {
        return Unexpected(JSError::range_error(js, std::format("Expected {} arguments, got {}", N, argc)));
    }

    return [&]<size_t... Is>(std::index_sequence<Is...>) -> auto {
        using Tuple = std::tuple<Ts...>;
        auto try_all = [&]() -> JSResult<Tuple> {
            auto args = std::make_tuple(try_into<std::tuple_element_t<Is, Tuple>>(Value::borrowed(js, argv[Is]))...);

            if ((... || !std::get<Is>(args).has_value())) {
                std::optional<JSError> err;
                (void)((std::get<Is>(args).has_value() ? false : (err = std::get<Is>(args).error(), true)) || ...);
                return Unexpected(*err);
            }
            return std::make_tuple(std::move(*std::get<Is>(args))...);
        };

        return try_all();
    }(std::make_index_sequence<N> {});
}

} // namespace muen::js

namespace muen {

template<typename T>
auto err(const js::JSResult<T>& r) noexcept -> Unexpected<Error> {
    return Unexpected(static_cast<Error>(std::make_shared<js::JSError>(r.error())));
}

} // namespace muen

#pragma once

#include <variant>

#include <fmt/format.h>

#include <defer.hpp>
#include <error.hpp>
#include <quickjs/value.hpp>

namespace glint::js {

class JSError: public glint::error::IError {
  public:
    explicit JSError(Value&& val, std::source_location loc = std::source_location::current()) noexcept :
        _val(std::move(val)),
        _loc(loc) {}

    ~JSError() override = default;
    JSError(const JSError&) = default;
    JSError(JSError&&) = default;
    auto operator=(const JSError&) -> JSError& = default;
    auto operator=(JSError&&) -> JSError& = default;

    static auto plain_error(
        not_null<JSContext *> ctx,
        const std::string& msg,
        std::source_location loc = std::source_location::current()
    ) noexcept -> JSError {
        auto eraw = JS_NewPlainError(ctx, "%s", msg.c_str());
        auto val = own(ctx, eraw);
        return JSError(std::move(val), loc);
    }

    static auto type_error(
        not_null<JSContext *> ctx,
        const std::string& msg,
        std::source_location loc = std::source_location::current()
    ) noexcept -> JSError {
        auto eraw = JS_NewTypeError(ctx, "%s", msg.c_str());
        auto val = own(ctx, eraw);
        return JSError(std::move(val), loc);
    }

    static auto range_error(
        not_null<JSContext *> ctx,
        const std::string& msg,
        std::source_location loc = std::source_location::current()
    ) noexcept -> JSError {
        auto eraw = JS_NewRangeError(ctx, "%s", msg.c_str());
        auto val = own(ctx, eraw);
        return JSError(std::move(val), loc);
    }

    [[nodiscard]]
    auto msg() const noexcept -> std::string override try {
        auto m = message().value_or("Unknown error");
        auto s = stack();

        if (s) {
            return fmt::format("{}\n{}", m, *s);
        } else {
            return m;
        }
    } catch (...) {
        return {};
    }

    [[nodiscard]]
    auto loc() const noexcept -> std::optional<std::source_location> override {
        return _loc;
    }

    [[nodiscard]]
    auto get() noexcept -> Value& {
        return _val;
    }

    [[nodiscard]]
    auto cget() const noexcept -> const Value& {
        return _val;
    }

    [[nodiscard]]
    auto message() const noexcept -> std::optional<std::string> try {
        auto message_val = JS_GetPropertyStr(_val.ctx(), _val.cget(), "message");
        defer(JS_FreeValue(_val.ctx(), message_val));

        auto len = size_t {};
        auto message_cstr = JS_ToCStringLen(_val.ctx(), &len, message_val);
        if (message_cstr == nullptr) return std::nullopt;
        defer(JS_FreeCString(_val.ctx(), message_cstr));

        return std::string {message_cstr, len};
    } catch (...) {
        return std::nullopt;
    };

    [[nodiscard]]
    auto stack() const noexcept -> std::optional<std::string> try {
        auto stack_val = JS_GetPropertyStr(_val.ctx(), _val.cget(), "stack");
        defer(JS_FreeValue(_val.ctx(), stack_val));

        auto len = size_t {};
        auto stack_cstr = JS_ToCStringLen(_val.ctx(), &len, stack_val);
        if (stack_cstr == nullptr) return std::nullopt;
        defer(JS_FreeCString(_val.ctx(), stack_cstr));

        return std::string {stack_cstr, len};
    } catch (...) {
        return std::nullopt;
    }

  private:
    Value _val;
    std::source_location _loc;
};

template<typename T = std::monostate>
using JSResult = std::expected<T, JSError>;

template<typename T>
concept is_jsresult = requires(T t, T::error_type e) {
    typename T::value_type;
    typename T::error_type;
    requires std::is_same_v<typename T::error_type, JSError>;
    requires std::is_constructible_v<T, typename T::value_type>;
    requires std::is_constructible_v<T, std::unexpected<typename T::error_type>>;
};

inline auto jsthrow(const JSError& e) noexcept -> JSValue {
    auto ctx = e.cget().ctx();
    return JS_Throw(ctx, JS_DupValue(ctx, e.cget().cget()));
}

} // namespace glint::js

namespace glint {

[[nodiscard]]
inline auto err(js::JSError e) noexcept -> Unexpected<Error> {
    return Unexpected(static_cast<Error>(std::make_shared<js::JSError>(std::move(e))));
}

template<typename T>
auto err(const js::JSResult<T>& r) noexcept -> Unexpected<Error> {
    return Unexpected(static_cast<Error>(std::make_shared<js::JSError>(r.error())));
}

} // namespace glint

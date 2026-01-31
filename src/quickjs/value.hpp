#pragma once

#include <variant>

#include <quickjs.h>

#include <quickjs/types.hpp>
#include <quickjs/utils.hpp>

namespace glint::js {

class Value {
  private:
    class Owned {
      public:
        friend class Value;

        Owned(not_null<JSContext *> js, JSValue value) noexcept : _ctx(js), _value(value) {}

        ~Owned() noexcept {
            if (_value) JS_FreeValue(_ctx, *_value);
        }

        Owned(const Value::Owned& other) noexcept : _ctx(other._ctx) {
            if (!other._value) return;
            _value = JS_DupValue(other._ctx, *other._value);
        }

        Owned(Value::Owned&& other) noexcept : _ctx(other._ctx) {
            if (!other._value) return;
            _value = other._value;
            other._value = std::nullopt;
        }

        auto operator=(const Value::Owned& other) noexcept -> Value::Owned& {
            if (this == &other) return *this;
            if (_value) JS_FreeValue(_ctx, *_value);
            _value = std::nullopt;

            if (!other._value) return *this;

            _ctx = other._ctx;
            _value = JS_DupValue(_ctx, *other._value);

            return *this;
        }

        auto operator=(Value::Owned&& other) noexcept -> Value::Owned& {
            using std::swap;
            swap(*this, other);
            return *this;
        }

        auto take() noexcept -> JSValue {
            if (!_value) return JS_UNINITIALIZED;
            auto val = *_value;
            _value.reset();
            return val;
        }

        friend inline auto swap(Owned& a, Owned& b) noexcept -> void;

      private:
        JSContext *_ctx;
        std::optional<JSValue> _value {};
    };

    class Borrowed {
      public:
        friend class Value;

        Borrowed(not_null<JSContext *> ctx, JSValue value) noexcept : _ctx(ctx), _value(value) {}

      private:
        JSContext *_ctx;
        JSValue _value {};
    };

  public:
    static auto owned(not_null<JSContext *> ctx, JSValue val) noexcept -> Value { return Value::Owned(ctx, val); }

    static auto borrowed(not_null<JSContext *> ctx, JSValue val) noexcept -> Value { return Value::Borrowed(ctx, val); }

    [[nodiscard]]
    auto ctx() const noexcept -> not_null<JSContext *> {
        if (auto v = std::get_if<Owned>(&_v)) return v->_ctx;
        if (auto v = std::get_if<Borrowed>(&_v)) return v->_ctx;
        std::unreachable();
    }

    [[nodiscard]]
    auto get() noexcept -> JSValue& {
        if (auto v = std::get_if<Owned>(&_v)) return *v->_value; // NOLINT
        if (auto v = std::get_if<Borrowed>(&_v)) return v->_value;
        std::unreachable();
    }

    [[nodiscard]]
    auto cget() const noexcept -> const JSValue& {
        if (auto v = std::get_if<Owned>(&_v)) return *v->_value; // NOLINT
        if (auto v = std::get_if<Borrowed>(&_v)) return v->_value;
        std::unreachable();
    }

    [[nodiscard]]
    auto take() noexcept -> JSValue {
        if (auto v = std::get_if<Owned>(&_v)) return v->take(); // NOLINT
        if (auto v = std::get_if<Borrowed>(&_v)) return v->_value;
        std::unreachable();
    }

    friend auto swap(Owned& a, Owned& b) noexcept -> void;

  private:
    std::variant<Owned, Borrowed> _v;

    Value(Owned&& value) noexcept : _v(std::move(value)) {}

    Value(Borrowed value) noexcept : _v(value) {}
};

inline auto swap(Value::Owned& a, Value::Owned& b) noexcept -> void {
    using std::swap;
    swap(a._ctx, b._ctx);
    swap(a._value, b._value);
}

inline auto borrow(not_null<JSContext *> ctx, JSValue val) noexcept -> Value {
    return Value::borrowed(ctx, val);
}

inline auto own(not_null<JSContext *> ctx, JSValue val) noexcept -> Value {
    return Value::owned(ctx, val);
}

constexpr inline auto display_type(const Value& val) -> czstring {
    return display_type(val.cget().tag);
}

} // namespace glint::js

#pragma once

#include <gsl/gsl>

#include <fmt/format.h>

#include <quickjs/error.hpp>
#include <quickjs/value.hpp>

namespace glint::js {

using namespace gsl;

class Object {
  public:
    auto static from_value(Value v) noexcept -> JSResult<Object> {
        if (!JS_IsObject(v.cget())) {
            return Unexpected(
                JSError::type_error(v.ctx(), fmt::format("Value of type '{}' is not an object", display_type(v)))
            );
        }

        return Object(std::move(v));
    }

    [[nodiscard]]
    auto get() noexcept -> Value& {
        return _value;
    }

    [[nodiscard]]
    auto cget() const noexcept -> const Value& {
        return _value;
    }

    template<typename T>
    [[nodiscard]]
    auto at(czstring name) const noexcept -> JSResult<T> {
        auto prop = Value::owned(_value.ctx(), JS_GetPropertyStr(_value.ctx(), _value.cget(), name));
        return convert_from_js<T>(prop);
    }

    template<typename T>
    [[nodiscard]]
    auto at(const std::string& name) const noexcept -> JSResult<T> {
        return at<T>(name.c_str());
    }

  private:
    Value _value;

    explicit Object(Value&& value) noexcept : _value(std::move(value)) {}
};

} // namespace glint::js

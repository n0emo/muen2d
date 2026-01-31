#pragma once

#include <quickjs/value.hpp>
#include <quickjs/error.hpp>

namespace glint::js {

class Function {
  private:
    Value _value;

  public:
    static auto from_value(Value value) noexcept -> JSResult<Function> {
        if (!JS_IsFunction(value.ctx(), value.cget())) {
            return Unexpected(JSError::type_error(value.ctx(), "Not a function"));
        }

        return Function(std::move(value));
    }

    auto operator()() noexcept -> JSResult<Value> { return this->operator()(JS_UNDEFINED, {}); }

    auto operator()(std::span<Value> args) noexcept -> JSResult<Value> { return this->operator()(JS_UNDEFINED, args); }

    auto operator()(JSValueConst this_value, std::span<Value> args) noexcept -> JSResult<Value> {
        auto argv = std::make_unique<JSValue[]>(args.size());
        for (size_t i = 0; i < args.size(); i++) {
            argv[i] = args[i].cget();
        }
        auto val = JS_Call(_value.ctx(), _value.cget(), this_value, int(args.size()), argv.get());
        if (JS_HasException(_value.ctx())) {
            auto ex = Value::owned(_value.ctx(), JS_GetException(_value.ctx()));
            return Unexpected(JSError(std::move(ex)));
        }
        return Value::owned(_value.ctx(), val);
    }

  private:
    Function(Value&& value) noexcept : _value(std::move(value)) {}
};

} // namespace glint::js

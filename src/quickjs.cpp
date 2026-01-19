#include <quickjs.hpp>

#include <cassert>
#include <utility>

namespace muen::js {

Value::Owned::Owned(not_null<JSContext *> js, JSValue value) noexcept : _ctx(js), _value(value) {};

Value::Owned::~Owned() noexcept {
    if (_value) JS_FreeValue(_ctx, *_value);
}

Value::Owned::Owned(const Value::Owned& other) noexcept : _ctx(other._ctx) {
    if (!other._value) return;
    _value = JS_DupValue(other._ctx, *other._value);
}

Value::Owned::Owned(Value::Owned&& other) noexcept : _ctx(other._ctx) {
    if (!other._value) return;
    _value = other._value;
    other._value = std::nullopt;
}

auto Value::Owned::operator=(const Value::Owned& other) noexcept -> Value::Owned& {
    if (this == &other) return *this;
    if (_value) JS_FreeValue(_ctx, *_value);
    _value = std::nullopt;

    if (!other._value) return *this;

    _ctx = other._ctx;
    _value = JS_DupValue(_ctx, *other._value);

    return *this;
}

auto Value::Owned::operator=(Value::Owned&& other) noexcept -> Value::Owned& {
    using std::swap;
    swap(*this, other);
    return *this;
}

auto swap(Value::Owned& a, Value::Owned& b) noexcept -> void {
    using std::swap;
    swap(a._ctx, b._ctx);
    swap(a._value, b._value);
}

Value::Borrowed::Borrowed(not_null<JSContext *> ctx, JSValue value) noexcept : _ctx(ctx), _value(value) {}

auto Value::owned(not_null<JSContext *> ctx, JSValue val) noexcept -> Value {
    return Value::Owned(ctx, val);
}

auto Value::borrowed(not_null<JSContext *> ctx, JSValue val) noexcept -> Value {
    return Value::Borrowed(ctx, val);
}

auto Value::ctx() const noexcept -> not_null<JSContext *> {
    if (auto v = std::get_if<Owned>(&_v)) return v->_ctx;
    if (auto v = std::get_if<Borrowed>(&_v)) return v->_ctx;
    std::unreachable();
}

auto Value::get() noexcept -> JSValue& {
    if (auto v = std::get_if<Owned>(&_v)) return *v->_value; // NOLINT
    if (auto v = std::get_if<Borrowed>(&_v)) return v->_value;
    std::unreachable();
}

auto Value::cget() const noexcept -> const JSValue& {
    if (auto v = std::get_if<Owned>(&_v)) return *v->_value; // NOLINT
    if (auto v = std::get_if<Borrowed>(&_v)) return v->_value;
    std::unreachable();
}

Value::Value(Owned&& value) noexcept : _v(std::move(value)) {}

Value::Value(Borrowed value) noexcept : _v(value) {}

auto Function::from_value(Value value) noexcept -> JSResult<Function> {
    if (!JS_IsFunction(value.ctx(), value.cget()))
        return Unexpected(JSError::type_error(value.ctx(), "Not a function"));
    return Function(std::move(value));
}

auto Function::operator()() noexcept -> JSResult<Value> {
    return this->operator()(JS_UNDEFINED, {});
}

auto Function::operator()(std::span<Value> args) noexcept -> JSResult<Value> {
    return this->operator()(JS_UNDEFINED, args);
}

auto Function::operator()(JSValueConst this_value, std::span<Value> args) noexcept -> JSResult<Value> {
    auto argv = std::make_unique<JSValue[]>(args.size());
    for (size_t i = 0; i < args.size(); i++) {
        argv[i] = args[i].cget();
    }
    auto val = JS_Call(_value.ctx(), _value.cget(), this_value, int(args.size()), argv.get());
    if (JS_HasException(_value.ctx())) {
        // TODO: Is it really borrowed? Do we need to free exception?
        auto ex = Value::borrowed(_value.ctx(), JS_GetException(_value.ctx()));
        return Unexpected(JSError::from_value(ex));
    }
    return Value::owned(_value.ctx(), val);
}

Function::Function(Value&& value) noexcept : _value(std::move(value)) {}

auto Object::from_value(Value v) noexcept -> JSResult<Object> {
    if (!JS_IsObject(v.cget())) {
        return Unexpected(
            JSError::type_error(v.ctx(), fmt::format("Value of type '{}' is not an object", display_type(v)))
        );
    }

    return Object(std::move(v));
}

auto Object::get() noexcept -> Value& {
    return _value;
}

auto Object::cget() const noexcept -> const Value& {
    return _value;
}

Object::Object(Value&& value) noexcept : _value(std::move(value)) {}

JSError::JSError(Object&& obj, std::source_location loc) noexcept : _obj(std::move(obj)), _loc(loc) {}

auto JSError::from_value(const Value& val, std::source_location loc) noexcept -> JSError {
    auto obj = Object::from_value(val);
    if (obj) {
        return JSError(std::move(*obj), loc);
    } else {
        return JSError::plain_error(val.ctx(), "Unknown error", loc);
    }
}

auto JSError::plain_error(not_null<JSContext *> ctx, const std::string& msg, std::source_location loc) noexcept
    -> JSError {
    auto eraw = JS_NewPlainError(ctx, "%s", msg.c_str());
    auto val = own(ctx, eraw);
    auto obj = *Object::from_value(std::move(val));
    return JSError(std::move(obj), loc);
}

auto JSError::type_error(not_null<JSContext *> ctx, const std::string& msg, std::source_location loc) noexcept
    -> JSError {
    auto eraw = JS_NewTypeError(ctx, "%s", msg.c_str());
    auto val = own(ctx, eraw);
    auto obj = *Object::from_value(std::move(val));
    return JSError(std::move(obj), loc);
}

auto JSError::range_error(not_null<JSContext *> ctx, const std::string& msg, std::source_location loc) noexcept
    -> JSError {
    auto eraw = JS_NewRangeError(ctx, "%s", msg.c_str());
    auto val = own(ctx, eraw);
    auto obj = *Object::from_value(std::move(val));
    return JSError(std::move(obj), loc);
}

auto JSError::msg() const noexcept -> std::string try {
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

auto JSError::loc() const noexcept -> std::optional<std::source_location> {
    return _loc;
}

auto JSError::get() noexcept -> Object& {
    return _obj;
}

auto JSError::cget() const noexcept -> const Object& {
    return _obj;
}

auto JSError::value() noexcept -> JSValue& {
    return _obj.get().get();
}

auto JSError::cvalue() const noexcept -> const JSValue& {
    return _obj.cget().cget();
}

auto JSError::message() const noexcept -> std::optional<std::string> try {
    auto r = _obj.at<std::optional<std::string>>("message");
    if (r.has_value() && r->has_value()) return *r;
    else return std::nullopt;
} catch (...) {
    return std::nullopt;
};

auto JSError::stack() const noexcept -> std::optional<std::string> try {
    auto r = _obj.at<std::optional<std::string>>("stack");
    if (r.has_value() && r->has_value()) return *r;
    else return std::nullopt;
} catch (...) {
    return std::nullopt;
}

auto borrow(not_null<JSContext *> ctx, JSValue val) noexcept -> Value {
    return Value::borrowed(ctx, val);
}

auto own(not_null<JSContext *> ctx, JSValue val) noexcept -> Value {
    return Value::owned(ctx, val);
}

auto jsthrow(JSError e) noexcept -> JSValue {
    auto ctx = e.cget().cget().ctx();
    return JS_Throw(ctx, JS_DupValue(ctx, e.value()));
}

auto to_string(JSContext *js, ::JSValueConst value) -> std::string {
    auto len = size_t {};
    auto cstr = ::JS_ToCStringLen(js, &len, value);
    if (cstr == nullptr) {
        return std::string {};
    }
    defer(::JS_FreeCString(js, cstr));
    return {cstr, len};
}

auto display_type(int64_t type) -> const char * {
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

auto display_type(JSValueConst val) -> czstring {
    return display_type(val.tag);
}

auto display_type(const Value& val) -> czstring {
    return display_type(val.cget().tag);
}

} // namespace muen::js

namespace muen {

auto err(js::JSError e) noexcept -> Unexpected<Error> {
    return Unexpected(static_cast<Error>(std::make_shared<js::JSError>(std::move(e))));
}

} // namespace muen

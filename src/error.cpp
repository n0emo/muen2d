#include <error.hpp>

#include <utility>
#include <format>

namespace muen::error {

auto IError::msg() const noexcept -> std::string_view {
    return "Generic error";
};

auto IError::loc() const noexcept -> std::optional<std::source_location> {
    return std::nullopt;
};

auto IError::loc_str() const noexcept -> std::optional<std::string> {
    if (auto loc = this->loc(); loc.has_value()) {
        return std::format("{}:{}:{} ({})", loc->file_name(), loc->line(), loc->column(), loc->function_name());
    } else {
        return std::nullopt;
    }
}

StringError::StringError(std::string msg, std::source_location loc) noexcept : _msg {std::move(msg)}, _loc {loc} {}

auto StringError::msg() const noexcept -> std::string_view {
    return _msg;
}

auto StringError::loc() const noexcept -> std::optional<std::source_location> {
    return _loc;
};

auto create(std::string message, std::source_location location) noexcept -> Error {
    return std::make_shared<StringError>(std::move(message), location);
}

auto create_ptr(std::string message, std::source_location location) noexcept -> IError * {
    return new (std::nothrow) StringError(std::move(message), location);
}

auto free_ptr(IError *e) noexcept -> void {
    delete e;
}

JsError::JsError(JSContext *js, JSValue value, std::source_location loc) noexcept :
    _js {js},
    _value {value},
    _loc {loc},
    _msg {nullptr} {
    try {
        auto msg = std::string {};
        if (auto s = str()) msg += *s;
        else msg += "<empty>";

        if (auto s = stack()) {
            msg += '\n';
            msg += *s;
        }
        _msg = std::make_shared<std::string>(std::move(msg));
    } catch (std::exception&) {
        _msg = std::make_shared<std::string>();
    }
}

JsError::JsError(const JsError& other) noexcept :
    _js {other._js},
    _value {JS_DupValue(other._js, other._value)},
    _loc {other._loc},
    _msg {other._msg} {}

JsError::~JsError() noexcept {
    if (JS_IsUninitialized(_value)) return;
    JS_FreeValue(_js, _value);
}

JsError::JsError(JsError&& other) noexcept :
    _js {other._js},
    _value {other._value},
    _loc {other._loc},
    _msg {other._msg} {
    other._value = JS_UNINITIALIZED;
}

auto JsError::operator=(const JsError& other) noexcept -> JsError& {
    if (this == &other) return *this;
    if (!JS_IsUninitialized(this->_value)) JS_FreeValue(this->_js, this->_value);

    this->_js = other._js;
    this->_value = JS_DupValue(other._js, other._value);
    this->_loc = other._loc;
    this->_msg = other._msg;

    return *this;
}

auto JsError::operator=(JsError&& other) noexcept -> JsError& {
    if (this == &other) return *this;
    if (!JS_IsUninitialized(this->_value)) JS_FreeValue(this->_js, this->_value);

    this->_js = other._js;
    this->_value = other._value;
    this->_loc = other._loc;
    this->_msg = std::move(other._msg);
    other._value = JS_UNINITIALIZED;

    return *this;
}

auto JsError::msg() const noexcept -> std::string_view {
    if (!_msg) {}

    return *_msg;
}

auto JsError::loc() const noexcept -> std::optional<std::source_location> {
    return _loc;
}

auto JsError::value() const noexcept -> JSValueConst {
    return _value;
}

// TODO: Return string_view to msg
auto JsError::str() const noexcept -> std::optional<std::string> {
    auto str = js::try_get_property<std::string>(_js, _value, "message");
    if (str) return *str;
    return std::nullopt;
}

// TODO: Return string_view to msg
auto JsError::stack() const noexcept -> std::optional<std::string> {
    auto stack = js::try_get_property<std::string>(_js, _value, "stack");
    if (stack) return *stack;
    return std::nullopt;
}

auto from_js(JSContext *js, JSValue value, std::source_location loc) noexcept -> Error {
    return std::make_shared<JsError>(js, value, loc);
}

auto from_js_ptr(JSContext *js, JSValue value, std::source_location loc) noexcept -> IError * {
    return new (std::nothrow) JsError(js, value, loc);
}

} // namespace muen::error

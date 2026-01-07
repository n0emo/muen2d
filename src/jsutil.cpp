#include <jsutil.hpp>

namespace js {

auto to_string(::JSContext *js, ::JSValueConst value) -> std::string {
    auto len = size_t {};
    auto cstr = ::JS_ToCStringLen(js, &len, value);
    if (cstr == nullptr) {
        return std::string {};
    }
    defer(::JS_FreeCString(js, cstr));
    return {cstr, len};
}

template<>
auto try_into<::JSValue>(::JSContext *js, JSValueConst value) -> std::expected<::JSValue, ::JSValue> {
    return value;
}

} // namespace js

#pragma once

#include <gsl/gsl>

#include <quickjs.h>

namespace glint::js {

using namespace gsl;

constexpr inline auto display_type(int64_t type) -> czstring {
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

constexpr inline auto display_type(JSValueConst val) -> czstring {
    return display_type(val.tag);
}

template<typename T>
auto class_id(not_null<JSRuntime *> rt) -> JSClassID {
    static auto id = JSClassID {0};
    JS_NewClassID(rt, &id);
    return id;
}

template<typename T>
auto class_id(not_null<JSContext *> js) -> JSClassID {
    return class_id<T>(JS_GetRuntime(js));
}

template<auto T>
auto class_id(not_null<JSRuntime *> rt) -> JSClassID {
    static auto id = JSClassID {0};
    JS_NewClassID(rt, &id);
    return id;
}

template<auto T>
auto class_id(not_null<JSContext *> js) -> JSClassID {
    return class_id<T>(JS_GetRuntime(js));
}

} // namespace glint::js

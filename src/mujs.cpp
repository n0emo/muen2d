#include "mujs.hpp"

#include <format>
#include <csetjmp>

#include <mujs.h>
#include <spdlog/spdlog.h>

#include "defer.hpp"

#define mujs_try(js)                                                                                                   \
    if (setjmp(*((jmp_buf *)::js_savetry((js).j)))) {                                                                  \
        throw Exception((js));                                                                                         \
    }                                                                                                                  \
    defer(::js_endtry((js).j))

namespace mujs {

Js Js::create() {
    auto j = ::js_newstate(nullptr, nullptr, JS_STRICT);

    return Js {.j = j};
}

void Js::destroy() {
    ::js_freestate(this->j);
}

ObjectBuilder Js::object(const char *prototype) {
    return ObjectBuilder::create(*this, prototype);
}

void Js::dump_error() {
    spdlog::error("Error initializing game: {}", js_tostring(j, -1));
    js_pop(j, 1);
}

void Js::eval_file(const char *path) {
    mujs_try(*this);
    ::js_loadfile(this->j, path);
    ::js_pushundefined(this->j);
    ::js_call(this->j, 0);
    ::js_pop(this->j, 1);
}

void Js::eval_string(const char *str) {
    mujs_try(*this);
    ::js_pushliteral(this->j, str);
    ::js_eval(this->j);
}

ObjectRef Js::get_global_object(const char *name) {
    return ObjectRef::get_global(*this, name);
}

ObjectBuilder ObjectBuilder::create(Js js, const char *prototype) {
    mujs_try(js);
    ::js_newobject(js.j);
    return ObjectBuilder {.js = js, .prototype = prototype};
}

ObjectBuilder& ObjectBuilder::define_method(void (*fun)(js_State *), const char *name, int length, int attrs) {
    mujs_try(this->js);

    auto fun_name = std::format("{}.prototype.{}", this->prototype, name);
    ::js_newcfunction(this->js.j, fun, fun_name.c_str(), length);
    ::js_defproperty(this->js.j, -2, name, attrs);

    return *this;
}

ObjectBuilder&
ObjectBuilder::define_accessor(void (*get)(js_State *), void (*set)(js_State *), const char *name, int attrs) {
    mujs_try(this->js);

    if (get != nullptr) {
        auto fun_name = std::format("{}.prototype.get_{}", this->prototype, name);
        ::js_newcfunction(this->js.j, get, fun_name.c_str(), 0);
    } else {
        ::js_pushnull(this->js.j);
    }

    if (set != nullptr) {
        auto fun_name = std::format("{}.prototype.set_{}", this->prototype, name);
        ::js_newcfunction(this->js.j, set, fun_name.c_str(), 1);
    } else {
        ::js_pushnull(this->js.j);
    }

    ::js_defaccessor(this->js.j, -3, name, attrs);

    return *this;
}

void ObjectBuilder::set_global(const char *name) {
    mujs_try(this->js);
    ::js_setglobal(this->js.j, name);
}

ObjectRef ObjectRef::get_global(Js js, const char *name) {
    mujs_try(js);
    ::js_getglobal(js.j, name);
    return ObjectRef {.js = js};
}

void ObjectRef::drop() {
    ::js_pop(this->js.j, 1);
}

std::optional<const char *> ObjectRef::get_string(const char *name) {
    mujs_try(this->js);
    if (!::js_hasproperty(this->js.j, -1, name)) {
        return std::nullopt;
    }

    if (!::js_isstring(this->js.j, -1)) {
        return std::nullopt;
    }

    auto result = js_tostring(this->js.j, -1);
    ::js_pop(this->js.j, 1);
    return result;
}

std::optional<int> ObjectRef::get_integer(const char *name) {
    mujs_try(this->js);
    if (!::js_hasproperty(this->js.j, -1, name)) {
        return std::nullopt;
    }

    if (!::js_isnumber(this->js.j, -1)) {
        ::js_pop(this->js.j, 1);
        return std::nullopt;
    }

    auto result = js_tointeger(this->js.j, -1);
    ::js_pop(this->js.j, 1);
    return result;
}

std::optional<ObjectRef> ObjectRef::get_object(const char *name) {
    mujs_try(this->js);
    if (!::js_hasproperty(this->js.j, -1, name)) {
        return std::nullopt;
    }

    if (!::js_isobject(this->js.j, -1)) {
        ::js_pop(this->js.j, -1);
        return std::nullopt;
    }

    return ObjectRef {.js = this->js};
}

Exception::Exception(Js& js) {
    auto stack_cstr = ::js_tostring(js.j, -1);
    ::js_pop(js.j, 1);
    this->stack = std::string {stack_cstr};
}

const char *Exception::what() const noexcept {
    return this->stack.c_str();
}

} // namespace mujs

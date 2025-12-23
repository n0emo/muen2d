#pragma once

#include <exception>
#include <string>
#include <optional>

struct js_State;

namespace mujs {

constexpr int READONLY = 1;
constexpr int DONTENUM = 2;
constexpr int DONTCONF = 4;

struct ObjectBuilder;
struct ObjectRef;

struct Js {
    js_State *j;

    static Js create();
    void destroy();

    ObjectBuilder object(const char *prototype);

    void dump_error();

    void eval_file(const char *path);
    void extracted();
    void eval_string(const char *str);

    ObjectRef get_global_object(const char *name);
};

struct ObjectBuilder {
    Js js;

    const char *prototype;

    static ObjectBuilder create(Js js, const char *prototype = "Object");

    ObjectBuilder& define_method(void (*fun)(js_State *), const char *name, int length, int attrs);
    ObjectBuilder& define_accessor(void (*get)(js_State *), void (*set)(js_State *), const char *name, int attrs);
    void set_global(const char *name);
};

struct ObjectRef {
    Js js;

    static ObjectRef get_global(Js js, const char *name);
    void drop();

    std::optional<const char *> get_string(const char *name);
    std::optional<int> get_integer(const char *name);
    std::optional<ObjectRef> get_object(const char *name);
};

struct Exception: public std::exception {
    std::string stack;

    Exception(Js&);
    const char *what() const noexcept override;
};

}; // namespace mujs

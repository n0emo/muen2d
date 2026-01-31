#pragma once

#include <array>

#include <spdlog/spdlog.h>
#include <boost/callable_traits.hpp>

#include <quickjs/convert.hpp>

namespace glint::js {

namespace callable_traits = boost::callable_traits;

template<typename T>
struct remove_first_type {};

template<typename T, typename... Ts>
struct remove_first_type<std::tuple<T, Ts...>> {
    using type = std::tuple<Ts...>;
};

namespace detail {
    template<typename Args>
    consteval auto js_arity_impl() noexcept -> size_t {
        constexpr auto length = std::tuple_size_v<Args>;

        if constexpr (length == 0 || std::is_same_v<Args, std::tuple<JSContext *, JSValueConst, int, JSValueConst>>) {
            return 0;
        } else if constexpr (length >= 2 && std::is_same_v<std::tuple_element_t<0, Args>, JSContext *>
                             && std::is_same_v<std::tuple_element_t<1, Args>, JSValueConst>) {
            return length - 2;
        } else if constexpr (length >= 1 && std::is_same_v<std::tuple_element_t<0, Args>, JSContext *>) {
            return length - 1;
        } else {
            return length;
        }
    }

    template<auto func>
    consteval auto js_arity() noexcept -> size_t {
        using Args = callable_traits::args_t<decltype(func)>;
        if constexpr (std::is_member_function_pointer_v<decltype(func)>) {
            return js_arity_impl<typename remove_first_type<Args>::type>();
        } else {
            return js_arity_impl<Args>();
        }
    }

    template<typename Ret, typename Args>
    auto call_convert_to_js_raw_throwing(auto func, JSContext *ctx, Args args) noexcept -> JSValue {
        if constexpr (std::is_same_v<Ret, void>) {
            std::apply(func, args);
            return JS_UNDEFINED;
        } else {
            auto val = std::apply(func, args);
            auto ret = convert_to_js(ctx, val);
            if (!ret) return jsthrow(ret.error());
            return ret->take();
        }
    }

    template<typename Args>
    auto get_args(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) noexcept -> JSResult<Args> {
        if constexpr (std::tuple_size_v<Args> == 0) {
            return std::make_tuple();
        } else if constexpr (std::is_same_v<Args, std::tuple<JSContext *, JSValueConst, int, JSValueConst>>) {
            return std::make_tuple(ctx, this_val, argc, argv);
        } else if constexpr (std::is_same_v<std::tuple_element_t<0, Args>, JSContext *>
                             && std::is_same_v<std::tuple_element_t<1, Args>, JSValueConst>) {
            using ArgsCpp = remove_first_type<typename remove_first_type<Args>::type>::type;
            auto args = unpack_args_t<ArgsCpp>(ctx, argc, argv);
            if (!args) return Unexpected(args.error());
            return std::tuple_cat(std::make_tuple(ctx, this_val), *args);
        } else if constexpr (std::is_same_v<std::tuple_element_t<0, Args>, JSContext *>) {
            using ArgsCpp = remove_first_type<Args>::type;
            auto args = unpack_args_t<ArgsCpp>(ctx, argc, argv);
            if (!args) return Unexpected(args.error());
            return std::tuple_cat(std::make_tuple(ctx), *args);
        } else {
            auto args = unpack_args_t<Args>(ctx, argc, argv);
            if (!args) return Unexpected(args.error());
            return *args;
        }
    }

    template<auto func>
    constexpr auto call(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) noexcept -> JSValue {
        using Args = callable_traits::args_t<decltype(func)>;
        using Ret = callable_traits::return_type<decltype(func)>::type;

        auto args = get_args<Args>(ctx, this_val, argc, argv);
        if (!args) return jsthrow(args.error());
        return call_convert_to_js_raw_throwing<Ret, Args>(func, ctx, *args);
    }

    template<typename T, auto func>
    constexpr auto call_method(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) noexcept
        -> JSValue {
        using FullArgs = callable_traits::args_t<decltype(func)>;
        using Args = remove_first_type<FullArgs>::type;
        using Ret = callable_traits::return_type<decltype(func)>::type;

        auto instance = T::get_instance(borrow(ctx, this_val));
        if (!instance) return jsthrow(instance.error());
        auto method = std::bind_front(func, *instance);
        auto args = get_args<Args>(ctx, this_val, argc, argv);

        return call_convert_to_js_raw_throwing<Ret, Args>(method, ctx, *args);
    }

    template<typename T, auto func>
    constexpr auto call_getter(JSContext *ctx, JSValueConst this_val) noexcept -> JSValue {
        return detail::call_method<T, func>(ctx, this_val, 0, nullptr);
    }

} // namespace detail

template<auto func>
constexpr auto js_func() -> auto {
    return [](JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) noexcept -> JSValue {
        return detail::call<func>(ctx, this_val, argc, argv);
    };
}

using PropertyList = std::initializer_list<JSCFunctionListEntry>;

/// Override:
/// - class_name
/// - initialize
/// - static_properties
/// - instance_properties
template<typename T>
class JSClass {
  public:
    friend T;

    constexpr static JSClassFinalizer *class_finalizer = [](JSRuntime *rt, JSValueConst val) noexcept -> void {
        auto ptr = static_cast<T *>(JS_GetOpaque(val, T::class_id(rt)));
        if (ptr == nullptr) {
            // NOLINTNEXTLINE(bugprone-lambda-function-name): at least filename will be useful
            SPDLOG_WARN("Could not finalize instance of {}: opaque pointer is null", T::class_name);
            return;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory): we cannot express ownership of JS object
        delete ptr;
    };

    constexpr static JSClassGCMark *class_gc_mark = nullptr;

    constexpr static JSClassCall *class_call = nullptr;

    constexpr static JSClassExoticMethods *class_exotic = nullptr;

    static constexpr JSClassDef class_def = {
        .class_name = T::class_name,
        .finalizer = T::class_finalizer,
        .gc_mark = T::class_gc_mark,
        .call = T::class_call,
        .exotic = T::class_exotic,
    };

    template<typename ArgsTuple>
    static auto create_instance_proto_t(const Value& proto, ArgsTuple args) noexcept -> JSValue {
        auto obj = JS_NewObjectProtoClass(proto.ctx(), proto.cget(), T::class_id(proto.ctx()));
        if (JS_HasException(proto.ctx())) {
            return JS_UNDEFINED;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory): we cannot express ownership of JS object
        auto ptr = new (std::nothrow) T();
        auto init = std::bind_front(&T::initialize, ptr);
        std::apply(init, args);
        JS_SetOpaque(obj, ptr);
        return obj;
    }

    template<typename... Args>
    static auto create_instance_proto(const Value& proto, Args... args) noexcept -> JSValue {
        return create_instance_proto_t(proto, std::make_tuple(args...));
    }

    template<typename ArgsTuple>
    static auto create_instance_this_t(const Value& this_val, ArgsTuple args) noexcept -> JSValue {
        auto proto = JS_GetPropertyStr(this_val.ctx(), this_val.cget(), "prototype");
        if (JS_HasException(this_val.ctx())) return JS_UNDEFINED;
        defer(JS_FreeValue(this_val.ctx(), proto));
        return create_instance_proto_t(borrow(this_val.ctx(), proto), args);
    }

    template<typename... Args>
    static auto create_instance_this(const Value& this_val, Args... args) noexcept -> JSValue {
        return create_instance_this_t(this_val, std::make_tuple(args...));
    }

    template<typename ArgsTuple>
    static auto create_instance_t(not_null<JSContext *> ctx, ArgsTuple args) noexcept -> JSValue {
        auto obj = JS_NewObjectClass(ctx, T::class_id(ctx));
        if (JS_HasException(ctx)) {
            return JS_UNDEFINED;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory): we cannot express ownership of JS object
        auto ptr = new (std::nothrow) T();
        auto init = std::bind_front(&T::initialize, ptr);
        std::apply(init, args);
        JS_SetOpaque(obj, ptr);
        return obj;
    }

    template<typename... Args>
    static auto create_instance(not_null<JSContext *> ctx, Args... args) noexcept -> JSValue {
        return create_instance_t(ctx, std::make_tuple(args...));
    }

    static constexpr JSCFunction *constructor =
        [](JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) noexcept -> JSValue {
        using initialize_args = remove_first_type<callable_traits::args_t<decltype(&T::initialize)>>::type;

        auto args = unpack_args_t<initialize_args>(ctx, argc, argv);
        if (!args) return jsthrow(args.error());

        auto proto = JS_GetPropertyStr(ctx, this_val, "prototype");
        if (JS_HasException(ctx)) {
            return JS_UNDEFINED;
        }
        defer(JS_FreeValue(ctx, proto));

        return create_instance_proto_t(borrow(ctx, proto), *args);
    };

    static constexpr std::initializer_list<JSCFunctionListEntry> static_properties = {};

    static constexpr std::initializer_list<JSCFunctionListEntry> instance_properties = {};

    static auto class_id(not_null<JSContext *> ctx) noexcept -> JSClassID { return js::class_id<T>(ctx); }

    static auto class_id(not_null<JSRuntime *> rt) noexcept -> JSClassID { return js::class_id<T>(rt); }

    static auto define(JSContext *ctx) noexcept -> Value {
        constexpr auto arity = std::tuple_size<typename callable_traits::args_t<decltype(&T::initialize)>>();

        JS_NewClass(JS_GetRuntime(ctx), T::class_id(ctx), &T::class_def);

        JSValue proto = JS_NewObject(ctx);
        JS_SetPropertyFunctionList(ctx, proto, T::instance_properties.begin(), int(T::instance_properties.size()));
        JS_SetClassProto(ctx, T::class_id(ctx), proto);

        JSValue ctor = JS_NewCFunction2(ctx, constructor, T::class_name, arity, JS_CFUNC_constructor, 0);
        JS_SetPropertyFunctionList(ctx, ctor, T::static_properties.begin(), int(T::static_properties.size()));
        JS_SetConstructor(ctx, ctor, proto);

        return own(ctx, ctor);
    }

    static auto get_instance(const Value& val) noexcept -> JSResult<T *> {
        auto ptr = static_cast<T *>(JS_GetOpaque(val.cget(), T::class_id(val.ctx())));
        if (!ptr)
            return Unexpected(JSError::type_error(val.ctx(), fmt::format("Not an instance of {}", T::class_name)));
        return ptr;
    }

  private:
    JSClass() = default;

    template<auto member>
    static consteval auto export_static_const(czstring name, uint8_t flags = 0) noexcept -> JSCFunctionListEntry {
        using M = std::decay_t<decltype(member)>;
        if constexpr (std::is_same_v<M, czstring>) {
            return JS_PROP_STRING_DEF(name, member, flags);
        } else if constexpr (std::is_same_v<M, int32_t>) {
            return JS_PROP_INT32_DEF(name, member, flags);
        } else if constexpr (std::is_same_v<M, int64_t>) {
            return JS_PROP_INT64_DEF(name, member, flags);
        } else if constexpr (std::is_same_v<M, double>) {
            return JS_PROP_DOUBLE_DEF(name, member, flags);
        } else if constexpr (std::is_same_v<M, uint64_t>) {
            return JS_PROP_U2D_DEF(name, member, flags);
        } else {
            static_assert(false, "Type not supported");
        }
    }

    template<auto member>
    static consteval auto export_static_method(czstring name, uint8_t flags = 0) noexcept -> JSCFunctionListEntry {
        constexpr auto arity = detail::js_arity<member>();
        auto func = js_func<member>();
        return JS_CFUNC_DEF2(name, arity, func, flags);
    }

    template<auto member>
    static consteval auto export_method(czstring name, uint8_t flags = 0) noexcept -> JSCFunctionListEntry {
        constexpr auto arity = detail::js_arity<member>();

        constexpr auto func =
            [](JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) noexcept -> JSValue {
            return detail::call_method<T, member>(ctx, this_val, argc, argv);
        };

        return JS_CFUNC_DEF2(name, arity, func, flags);
    }

    template<auto getter>
    static consteval auto export_get_only(czstring name, uint8_t flags = 0) noexcept -> JSCFunctionListEntry {
        auto get = make_js_getter_func<getter>();
        return JS_CGETSET_DEF2(name, get, nullptr, flags);
    }

    template<auto setter>
    static consteval auto export_set_only(czstring name, uint8_t flags = 0) noexcept -> JSCFunctionListEntry {
        auto set = make_js_setter_func<setter>();
        return JS_CGETSET_DEF2(name, nullptr, set, flags);
    }

    template<auto getter, auto setter>
    static consteval auto export_getset(czstring name, uint8_t flags = 0) noexcept -> JSCFunctionListEntry {
        auto get = make_js_getter_func<getter>();
        auto set = make_js_setter_func<setter>();
        return JS_CGETSET_DEF2(name, get, set, flags);
    }

    template<auto getter>
    static consteval auto make_js_getter_func() noexcept -> auto {
        // using Args = callable_traits::args_t<decltype(getter)>;
        // using Ret = callable_traits::return_type<decltype(getter)>::type;

        return [](JSContext *ctx, JSValueConst this_val) -> JSValue {
            return detail::call_getter<T, getter>(ctx, this_val);
            // auto instance = T::get_instance(ctx, borrow(ctx, this_val));
            // if (!instance) return jsthrow(instance.error());
            // auto method = std::bind_front(getter, *instance);
            // return detail::call_convert_to_js_raw_throwing<Ret, std::tuple<>>(method, ctx, std::make_tuple());
        };
    }

    template<auto setter>
    static consteval auto make_js_setter_func() -> auto {
        // TODO: Pass ctx, this_val and val if needed
        using F = decltype(setter);
        using FullArgs = callable_traits::args_t<F>;
        using Arg = std::tuple_element_t<1, FullArgs>;
        using Ret = callable_traits::return_type<decltype(setter)>::type;

        return [](JSContext *ctx, JSValueConst this_val, JSValueConst val) -> JSValue {
            auto instance = T::get_instance(borrow(ctx, this_val));
            if (!instance) return jsthrow(instance.error());
            auto arg = convert_from_js<Arg>(borrow(ctx, val));
            if (!arg) return jsthrow(arg.error());
            auto method = std::bind_front(setter, *instance);
            return detail::call_convert_to_js_raw_throwing<Ret, std::tuple<Arg>>(method, ctx, std::make_tuple(*arg));
        };
    }
};

} // namespace glint::js

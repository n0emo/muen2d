#pragma once

#include <gsl/gsl>
#include <optional>
#include <string>

namespace glint::js {

using namespace gsl;

template<class T>
struct is_basic_string: std::false_type {};

template<class C, class T, class A>
struct is_basic_string<std::basic_string<C, T, A>>: std::true_type {};

template<class T>
constexpr bool is_basic_string_v = is_basic_string<T>::value;

template<typename T>
concept is_container = requires(T a, typename T::value_type v) {
    typename T::value_type;
    { T {} } -> std::same_as<T>;
    { a.push_back(v) } -> std::same_as<void>;
};

template<typename T>
concept is_optional = requires(T a, typename T::value_type v) {
    typename T::value_type;
    requires std::is_constructible_v<T, std::nullopt_t>;
    requires std::is_constructible_v<T, typename T::value_type>;
};

} // namespace glint::js

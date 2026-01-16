#pragma once

#include <expected>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <variant>

#include <quickjs.h>

#include <defer.hpp>
#include <jsutil.hpp>

namespace muen::error {

/// Generic error interface
class IError {
  public:
    [[nodiscard]]
    virtual auto msg() const noexcept -> std::string_view;

    [[nodiscard]]
    virtual auto loc() const noexcept -> std::optional<std::source_location>;

    virtual ~IError() = default;

    [[nodiscard]]
    auto loc_str() const noexcept -> std::optional<std::string>;

    IError(const IError&) = delete;
    IError(IError&&) = delete;
    auto operator=(const IError&) -> IError& = delete;
    auto operator=(IError&&) -> IError& = delete;

  protected:
    IError() = default;
};

/// Error containing message as string
class StringError: public IError {
  private:
    std::string _msg;
    std::source_location _loc;

  public:
    StringError(std::string msg, std::source_location loc = std::source_location::current()) noexcept;

    [[nodiscard]]
    auto msg() const noexcept -> std::string_view override;

    [[nodiscard]]
    auto loc() const noexcept -> std::optional<std::source_location> override;
};

class JsError: public IError {
  private:
    JSContext *_js;
    JSValue _value;
    std::source_location _loc;
    std::shared_ptr<std::string> _msg;

  public:
    JsError(JSContext *js, JSValue value, std::source_location loc = std::source_location::current()) noexcept;
    ~JsError() noexcept override;
    JsError(const JsError& other) noexcept;
    JsError(JsError&& other) noexcept;
    auto operator=(const JsError& other) noexcept -> JsError&;
    auto operator=(JsError&& other) noexcept -> JsError&;

    [[nodiscard]]
    auto msg() const noexcept -> std::string_view override;

    [[nodiscard]]
    auto loc() const noexcept -> std::optional<std::source_location> override;

    [[nodiscard]]
    auto value() const noexcept -> JSValueConst;

    [[nodiscard]]
    auto str() const noexcept -> std::optional<std::string>;

    [[nodiscard]]
    auto stack() const noexcept -> std::optional<std::string>;
};

using Error = std::shared_ptr<IError>;

template<typename T = std::monostate, typename E = Error>
using Result = std::expected<T, Error>;

template<typename E>
using Err = std::unexpected<E>;

/// Create new error
[[nodiscard]]
auto create(std::string msg, std::source_location loc = std::source_location::current()) noexcept -> Error;

/// Create new error and return raw pointer
[[nodiscard]]
auto create_ptr(std::string msg = "", std::source_location loc = std::source_location::current()) noexcept -> IError *;

/// Free raw pointer to error
auto free_ptr(IError *e) noexcept -> void;

/// Create error from JS Value
[[nodiscard]]
auto from_js(JSContext *js, JSValue value, std::source_location loc = std::source_location::current()) noexcept
    -> Error;

/// Create new error from JS Value and return raw pointer
[[nodiscard]]
auto from_js_ptr(JSContext *js, JSValue value, std::source_location loc = std::source_location::current()) noexcept
    -> IError *;

} // namespace muen::error

namespace muen {

using error::Err;
using error::Error;
using error::Result;

} // namespace muen

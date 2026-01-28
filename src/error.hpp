#pragma once

#include <expected>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <variant>

#include <gsl/gsl>

#include <types.hpp>
#include <defer.hpp>

namespace glint::error {

using namespace gsl;

/// Generic error interface
class IError {
  public:
    [[nodiscard]]
    virtual auto msg() const noexcept -> std::string;

    [[nodiscard]]
    virtual auto loc() const noexcept -> std::optional<std::source_location>;

    virtual ~IError() = default;

    [[nodiscard]]
    auto loc_str() const noexcept -> std::optional<std::string>;

  protected:
    IError() = default;
    IError(const IError&) = default;
    IError(IError&&) = default;
    auto operator=(const IError&) -> IError& = default;
    auto operator=(IError&&) -> IError& = default;
};

/// Error containing message as string
class StringError: public IError {
  private:
    std::string _msg;
    std::source_location _loc;

  public:
    StringError(std::string msg, std::source_location loc = std::source_location::current()) noexcept;

    [[nodiscard]]
    auto msg() const noexcept -> std::string override;

    [[nodiscard]]
    auto loc() const noexcept -> std::optional<std::source_location> override;

    ~StringError() override = default;
    StringError(const StringError&) = default;
    StringError(StringError&&) = default;
    auto operator=(const StringError&) -> StringError& = default;
    auto operator=(StringError&&) -> StringError& = default;
};

class StdError: public IError {
  private:
    not_null<const std::exception *> _exception;
    std::source_location _loc;

  public:
    StdError(const std::exception& e, std::source_location loc);

    [[nodiscard]]
    auto msg() const noexcept -> std::string override;

    [[nodiscard]]
    auto loc() const noexcept -> std::optional<std::source_location> override;

    [[nodiscard]]
    auto exception() const noexcept -> const std::exception&;

    ~StdError() override = default;
    StdError(const StdError&) = default;
    StdError(StdError&&) = default;
    auto operator=(const StdError&) -> StdError& = default;
    auto operator=(StdError&&) -> StdError& = default;
};

using Error = std::shared_ptr<IError>;

template<typename E>
using Unexpected = std::unexpected<E>;

template<typename T = std::monostate, typename E = Error>
using Result = std::expected<T, E>;

/// Create new error
[[nodiscard]]
auto create(std::string msg, std::source_location loc = std::source_location::current()) noexcept -> Error;

/// Create new error and return raw pointer
[[nodiscard]]
auto create_ptr(std::string msg = "", std::source_location loc = std::source_location::current()) noexcept
    -> owner<IError *>;

/// Free raw pointer to error
auto free_ptr(owner<IError *> e) noexcept -> void;

} // namespace glint::error

namespace glint {

using error::Error;
using error::Result;
using error::Unexpected;

[[nodiscard]]
auto err(Error e) noexcept -> Unexpected<Error>;

[[nodiscard]]
auto err(std::string msg, std::source_location loc = std::source_location::current()) noexcept -> Unexpected<Error>;

[[nodiscard]]
auto err(std::exception& e, std::source_location loc = std::source_location::current()) noexcept -> Unexpected<Error>;

template<typename T, typename E>
auto err(const Result<T, E>& r) noexcept -> Unexpected<E> {
    return Unexpected(r.error());
}

} // namespace glint

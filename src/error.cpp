#include <error.hpp>

#include <utility>

#include <fmt/format.h>

namespace muen::error {

auto IError::msg() const noexcept -> std::string try { return "Generic error"; } catch (...) {
    return {};
};

auto IError::loc() const noexcept -> std::optional<std::source_location> {
    return std::nullopt;
};

auto IError::loc_str() const noexcept -> std::optional<std::string> {
    if (auto loc = this->loc(); loc.has_value()) {
        return fmt::format("{}:{}:{} ({})", loc->file_name(), loc->line(), loc->column(), loc->function_name());
    } else {
        return std::nullopt;
    }
}

StringError::StringError(std::string msg, std::source_location loc) noexcept : _msg {std::move(msg)}, _loc {loc} {}

auto StringError::msg() const noexcept -> std::string try { return _msg; } catch (...) {
    return {};
};

auto StringError::loc() const noexcept -> std::optional<std::source_location> {
    return _loc;
}

StdError::StdError(const std::exception& e, std::source_location loc) : _exception {&e}, _loc {loc} {}

auto StdError::msg() const noexcept -> std::string try { return _exception->what(); } catch (...) {
    return {};
}

auto StdError::loc() const noexcept -> std::optional<std::source_location> {
    return _loc;
}

auto StdError::exception() const noexcept -> const std::exception& {
    return *_exception;
}

auto create(std::string message, std::source_location location) noexcept -> Error {
    return std::make_shared<StringError>(std::move(message), location);
}

auto create_ptr(std::string message, std::source_location location) noexcept -> owner<IError *> {
    return new (std::nothrow) StringError(std::move(message), location);
}

auto free_ptr(owner<IError *> e) noexcept -> void {
    delete e;
}

} // namespace muen::error

namespace muen {

auto err(Error e) noexcept -> Unexpected<Error> {
    return Unexpected(std::move(e));
}

auto err(std::string msg, std::source_location loc) noexcept -> Unexpected<Error> {
    return Unexpected(static_cast<Error>(std::make_shared<error::StringError>(std::move(msg), loc)));
}

auto err(std::exception& e, std::source_location loc) noexcept -> Unexpected<Error> {
    return Unexpected(static_cast<Error>(std::make_shared<error::StdError>(e, loc)));
}

} // namespace muen

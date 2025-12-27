#include "js.hpp"

#include <format>

namespace muen::js {

auto tofloat(State *j, int idx) -> float {
    return static_cast<float>(tonumber(j, idx));
}

auto eval_file(State *j, const char *path) -> void {
    js::loadfile(j, path);
    js::pushundefined(j);
    js::call(j, 0);
    js::pop(j, 1);
}

Exception::Exception(js_State *j) {
    js::getproperty(j, -1, "stack");
    auto stack = js::tostring(j, -1);
    js::pop(j, 1);

    auto desc = js::tostring(j, -1);
    js::pop(j, 1);

    this->message = std::format("{}\nStack trace:{}", desc, stack);
}

auto Exception::what() const noexcept -> const char * {
    return this->message.c_str();
}

} // namespace muen::js

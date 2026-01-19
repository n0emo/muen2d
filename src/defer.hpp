#pragma once

#include <gsl/gsl>

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y) // NOLINT
#define DEFER_3(x) DEFER_2(x, __COUNTER__) // NOLINT
#define defer(code) auto DEFER_3(_defer_) = gsl::finally([&]() { code; }) // NOLINT

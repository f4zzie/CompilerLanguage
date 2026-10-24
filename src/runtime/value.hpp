#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace kite {

struct UnitValue {};
using Value = std::variant<std::int64_t, double, bool, std::string, UnitValue>;

std::string value_to_string(const Value& value);

}  // namespace kite

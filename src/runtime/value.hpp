#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace kite {

struct UnitValue {
  friend bool operator==(UnitValue, UnitValue) { return true; }
};
struct RuntimeArray;
using Value = std::variant<std::int64_t, double, bool, std::string, UnitValue,
                           std::shared_ptr<RuntimeArray>>;

struct RuntimeArray {
  std::vector<Value> elements;
};

std::string value_to_string(const Value& value);

}  // namespace kite

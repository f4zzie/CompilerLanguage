#pragma once

#include "runtime/value.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace kite {

using Builtin = std::function<Value(const std::vector<Value>&)>;

class BuiltinRegistry {
 public:
  void register_builtin(std::string name, Builtin function);
  [[nodiscard]] const Builtin* find(const std::string& name) const;

 private:
  std::unordered_map<std::string, Builtin> functions_;
};

}  // namespace kite

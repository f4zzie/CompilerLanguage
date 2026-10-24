#include "runtime/builtins.hpp"

#include <utility>

namespace kite {

void BuiltinRegistry::register_builtin(std::string name, Builtin function) {
  functions_[std::move(name)] = std::move(function);
}

const Builtin* BuiltinRegistry::find(const std::string& name) const {
  const auto iterator = functions_.find(name);
  return iterator == functions_.end() ? nullptr : &iterator->second;
}

}  // namespace kite

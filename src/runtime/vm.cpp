#include "runtime/vm.hpp"

#include <iomanip>
#include <sstream>
#include <type_traits>

namespace kite {

std::string value_to_string(const Value& value) {
  return std::visit(
      [](const auto& item) {
        using Item = std::decay_t<decltype(item)>;
        std::ostringstream output;
        if constexpr (std::is_same_v<Item, UnitValue>) {
          return std::string("unit");
        } else if constexpr (std::is_same_v<Item, bool>) {
          return item ? std::string("true") : std::string("false");
        } else {
          output << item;
          return output.str();
        }
      },
      value);
}

VmResult Vm::run(const BytecodeModule& module) {
  // TODO(T012): implement stack dispatch, call frames, builtins, and runtime diagnostics.
  if (module.instructions.empty()) {
    return VmResult{true, UnitValue{}, std::nullopt};
  }
  return VmResult{false, UnitValue{}, RuntimeError{"R999", "VM execution is not implemented", Span::synthetic()}};
}

}  // namespace kite

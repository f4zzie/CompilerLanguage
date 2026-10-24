#include "codegen/code_generator.hpp"

namespace kite {

BytecodeModule CodeGenerator::generate(const Program& /*program*/, Diagnostics& /*diagnostics*/) {
  // TODO(T011): lower the typed AST into deterministic bytecode and function metadata.
  return BytecodeModule{};
}

}  // namespace kite

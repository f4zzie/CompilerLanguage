#pragma once

#include "codegen/bytecode.hpp"
#include "diagnostics/diagnostic.hpp"
#include "parser/ast.hpp"

namespace kite {

class CodeGenerator {
 public:
  BytecodeModule generate(const Program& program, Diagnostics& diagnostics);
};

}  // namespace kite

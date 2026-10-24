#pragma once

#include "diagnostics/diagnostic.hpp"
#include "parser/ast.hpp"
#include "sema/symbol.hpp"

namespace kite {

struct SemanticResult {
  bool success{false};
};

class SemanticAnalyzer {
 public:
  SemanticResult analyze(Program& program, Diagnostics& diagnostics);
};

}  // namespace kite

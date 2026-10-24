#pragma once

#include "diagnostics/diagnostic.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"

#include <memory>
#include <vector>

namespace kite {

class Parser {
 public:
  std::unique_ptr<Program> parse(const std::vector<Token>& tokens, Diagnostics& diagnostics);
};

}  // namespace kite

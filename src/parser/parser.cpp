#include "parser/parser.hpp"

namespace kite {

std::unique_ptr<Program> Parser::parse(const std::vector<Token>& /*tokens*/,
                                       Diagnostics& /*diagnostics*/) {
  // TODO(T006): implement recursive-descent declarations/statements and Pratt expressions.
  return std::make_unique<Program>();
}

}  // namespace kite

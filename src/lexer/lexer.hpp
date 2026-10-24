#pragma once

#include "diagnostics/diagnostic.hpp"
#include "lexer/token.hpp"

#include <string_view>
#include <vector>

namespace kite {

class Lexer {
 public:
  std::vector<Token> scan(std::string_view file, std::string_view source, Diagnostics& diagnostics);
};

}  // namespace kite

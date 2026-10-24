#include "lexer/token_dump.hpp"

namespace kite {

std::string dump_tokens(const std::vector<Token>& /*tokens*/) {
  // TODO(T004): implement the stable token dump used by golden tests and `kite tokens`.
  return "EOF\n";
}

}  // namespace kite

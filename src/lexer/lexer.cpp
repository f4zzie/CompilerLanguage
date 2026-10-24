#include "lexer/lexer.hpp"

namespace kite {

std::vector<Token> Lexer::scan(std::string_view /*file*/, std::string_view /*source*/,
                               Diagnostics& /*diagnostics*/) {
  // TODO(T004): implement deterministic lexical scanning and source spans.
  return {Token{TokenKind::Eof, "", 0, 0.0, Span::synthetic()}};
}

const char* token_kind_name(TokenKind kind) {
  switch (kind) {
    case TokenKind::Eof:
      return "eof";
    case TokenKind::Identifier:
      return "identifier";
    case TokenKind::Integer:
      return "integer";
    case TokenKind::Float:
      return "float";
    case TokenKind::String:
      return "string";
    default:
      return "token";
  }
}

}  // namespace kite

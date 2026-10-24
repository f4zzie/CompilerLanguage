#include "lexer/token_dump.hpp"

#include <sstream>

namespace kite {
namespace {

std::string escape_lexeme(std::string_view lexeme) {
  std::string escaped;
  for (const char character : lexeme) {
    switch (character) {
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      case '`':
        escaped += "\\`";
        break;
      default:
        escaped.push_back(character);
        break;
    }
  }
  return escaped;
}

}  // namespace

std::string dump_tokens(const std::vector<Token>& tokens) {
  std::ostringstream output;
  for (const auto& token : tokens) {
    output << token.span.start.line << ':' << token.span.start.column << ' '
           << token_kind_name(token.kind) << " `" << escape_lexeme(token.lexeme) << "`\n";
  }
  return output.str();
}

}  // namespace kite

#include "lexer/lexer.hpp"

#include <charconv>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <string_view>
#include <unordered_map>

namespace kite {
namespace {

class Scanner {
 public:
  Scanner(std::string_view file, std::string_view source, Diagnostics& diagnostics)
      : file_(file), source_(source), diagnostics_(diagnostics) {}

  std::vector<Token> scan() {
    while (!at_end()) {
      skip_whitespace_and_comments();
      if (at_end()) {
        break;
      }
      scan_token();
    }

    const auto location = current_location();
    tokens_.push_back(Token{TokenKind::Eof, "", 0, 0.0, Span{location, location}});
    return tokens_;
  }

 private:
  bool at_end() const { return index_ >= source_.size(); }

  char peek(std::size_t distance = 0) const {
    const auto position = index_ + distance;
    return position < source_.size() ? source_[position] : '\0';
  }

  char advance() {
    const char character = peek();
    if (character == '\0') {
      return character;
    }
    ++index_;
    if (character == '\n') {
      ++line_;
      column_ = 1;
    } else {
      ++column_;
    }
    return character;
  }

  bool match(char expected) {
    if (peek() != expected) {
      return false;
    }
    advance();
    return true;
  }

  SourceLocation current_location() const {
    return SourceLocation{std::string(file_), line_, column_, index_};
  }

  Span span_from(const SourceLocation& start) const { return Span{start, current_location()}; }

  std::string source_slice(std::size_t start) const {
    return std::string(source_.substr(start, index_ - start));
  }

  void emit(TokenKind kind, const SourceLocation& start, std::string lexeme = {}) {
    if (lexeme.empty()) {
      lexeme = source_slice(start.offset);
    }
    tokens_.push_back(Token{kind, std::move(lexeme), 0, 0.0, span_from(start)});
  }

  void emit_integer(const SourceLocation& start) {
    const auto lexeme = source_slice(start.offset);
    std::int64_t value = 0;
    const auto result = std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), value);
    if (result.ec != std::errc{} || result.ptr != lexeme.data() + lexeme.size()) {
      diagnostics_.error("L001", "integer literal is out of range", span_from(start));
      value = 0;
    }
    tokens_.push_back(Token{TokenKind::Integer, lexeme, value, 0.0, span_from(start)});
  }

  void emit_float(const SourceLocation& start) {
    const auto lexeme = source_slice(start.offset);
    errno = 0;
    char* end = nullptr;
    const double value = std::strtod(lexeme.c_str(), &end);
    if (errno == ERANGE || end != lexeme.c_str() + lexeme.size()) {
      diagnostics_.error("L001", "float literal is out of range", span_from(start));
    }
    tokens_.push_back(Token{TokenKind::Float, lexeme, 0, value, span_from(start)});
  }

  void skip_whitespace_and_comments() {
    bool made_progress = true;
    while (made_progress && !at_end()) {
      made_progress = false;
      while (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n') {
        advance();
        made_progress = true;
      }
      if (peek() != '/' || peek(1) != '/') {
        if (peek() == '/' && peek(1) == '*') {
          const auto start = current_location();
          advance();
          advance();
          while (!at_end() && !(peek() == '*' && peek(1) == '/')) {
            advance();
          }
          if (at_end()) {
            diagnostics_.error("L002", "unterminated block comment", span_from(start));
          } else {
            advance();
            advance();
          }
          made_progress = true;
        }
        continue;
      }
      while (!at_end() && peek() != '\n') {
        advance();
      }
      made_progress = true;
    }
  }

  void scan_token() {
    const auto start = current_location();
    const char character = advance();

    if (is_identifier_start(character)) {
      while (is_identifier_continue(peek())) {
        advance();
      }
      emit(keyword_kind(source_slice(start.offset)), start);
      return;
    }

    if (is_digit(character)) {
      while (is_digit(peek())) {
        advance();
      }
      if (peek() == '.' && is_digit(peek(1))) {
        advance();
        while (is_digit(peek())) {
          advance();
        }
        emit_float(start);
      } else {
        emit_integer(start);
      }
      return;
    }

    if (character == '"') {
      scan_string(start);
      return;
    }

    switch (character) {
      case '(':
        emit(TokenKind::LeftParen, start);
        return;
      case ')':
        emit(TokenKind::RightParen, start);
        return;
      case '{':
        emit(TokenKind::LeftBrace, start);
        return;
      case '}':
        emit(TokenKind::RightBrace, start);
        return;
      case '[':
        emit(TokenKind::LeftBracket, start);
        return;
      case ']':
        emit(TokenKind::RightBracket, start);
        return;
      case ',':
        emit(TokenKind::Comma, start);
        return;
      case ':':
        emit(TokenKind::Colon, start);
        return;
      case ';':
        emit(TokenKind::Semicolon, start);
        return;
      case '+':
        emit(TokenKind::Plus, start);
        return;
      case '-':
        emit(match('>') ? TokenKind::Arrow : TokenKind::Minus, start);
        return;
      case '*':
        emit(TokenKind::Star, start);
        return;
      case '%':
        emit(TokenKind::Percent, start);
        return;
      case '/':
        emit(TokenKind::Slash, start);
        return;
      case '=':
        emit(match('=') ? TokenKind::EqualEqual : TokenKind::Equal, start);
        return;
      case '!':
        emit(match('=') ? TokenKind::BangEqual : TokenKind::Not, start);
        return;
      case '<':
        emit(match('=') ? TokenKind::LessEqual : TokenKind::Less, start);
        return;
      case '>':
        emit(match('=') ? TokenKind::GreaterEqual : TokenKind::Greater, start);
        return;
      case '|':
        if (match('>')) {
          emit(TokenKind::Pipe, start);
          return;
        }
        break;
      default:
        break;
    }

    diagnostics_.error("L001", "illegal character '" + std::string(1, character) + "'",
                       span_from(start));
  }

  void scan_string(const SourceLocation& start) {
    std::string value;
    bool terminated = false;
    while (!at_end()) {
      const char character = advance();
      if (character == '"') {
        terminated = true;
        break;
      }
      if (character == '\n') {
        diagnostics_.error("L003", "unterminated string literal", span_from(start));
        break;
      }
      if (character != '\\') {
        value.push_back(character);
        continue;
      }
      if (at_end()) {
        break;
      }
      const char escaped = advance();
      switch (escaped) {
        case '\\':
          value.push_back('\\');
          break;
        case '"':
          value.push_back('"');
          break;
        case 'n':
          value.push_back('\n');
          break;
        case 'r':
          value.push_back('\r');
          break;
        case 't':
          value.push_back('\t');
          break;
        default:
          diagnostics_.error("L004", "invalid escape sequence", span_from(start));
          value.push_back(escaped);
          break;
      }
    }
    if (!terminated && at_end() && (source_.empty() || source_[index_ - 1] != '\n')) {
      diagnostics_.error("L003", "unterminated string literal", span_from(start));
    }
    if (terminated) {
      emit(TokenKind::String, start, std::move(value));
    }
  }

  static bool is_digit(char character) {
    return character >= '0' && character <= '9';
  }

  static bool is_identifier_start(char character) {
    return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
           character == '_';
  }

  static bool is_identifier_continue(char character) {
    return is_identifier_start(character) || is_digit(character);
  }

  static TokenKind keyword_kind(std::string_view text) {
    static const std::unordered_map<std::string_view, TokenKind> keywords{
        {"true", TokenKind::True},       {"false", TokenKind::False},
        {"unit", TokenKind::Unit},       {"fn", TokenKind::Fn},
        {"let", TokenKind::Let},         {"if", TokenKind::If},
        {"else", TokenKind::Else},       {"while", TokenKind::While},
        {"for", TokenKind::For},         {"return", TokenKind::Return},
        {"int", TokenKind::IntType},     {"float", TokenKind::FloatType},
        {"bool", TokenKind::BoolType},   {"string", TokenKind::StringType},
        {"and", TokenKind::And},         {"or", TokenKind::Or},
        {"not", TokenKind::Not},
    };
    const auto iterator = keywords.find(text);
    return iterator == keywords.end() ? TokenKind::Identifier : iterator->second;
  }

  std::string_view file_;
  std::string_view source_;
  Diagnostics& diagnostics_;
  std::size_t index_{0};
  std::size_t line_{1};
  std::size_t column_{1};
  std::vector<Token> tokens_;
};

}  // namespace

std::vector<Token> Lexer::scan(std::string_view file, std::string_view source,
                               Diagnostics& diagnostics) {
  return Scanner(file, source, diagnostics).scan();
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
    case TokenKind::True:
      return "true";
    case TokenKind::False:
      return "false";
    case TokenKind::Unit:
      return "unit";
    case TokenKind::Fn:
      return "fn";
    case TokenKind::Let:
      return "let";
    case TokenKind::If:
      return "if";
    case TokenKind::Else:
      return "else";
    case TokenKind::While:
      return "while";
    case TokenKind::For:
      return "for";
    case TokenKind::Return:
      return "return";
    case TokenKind::IntType:
      return "int";
    case TokenKind::FloatType:
      return "float";
    case TokenKind::BoolType:
      return "bool";
    case TokenKind::StringType:
      return "string";
    case TokenKind::UnitType:
      return "unit_type";
    case TokenKind::And:
      return "and";
    case TokenKind::Or:
      return "or";
    case TokenKind::Not:
      return "not";
    case TokenKind::Plus:
      return "+";
    case TokenKind::Minus:
      return "-";
    case TokenKind::Star:
      return "*";
    case TokenKind::Slash:
      return "/";
    case TokenKind::Percent:
      return "%";
    case TokenKind::Equal:
      return "=";
    case TokenKind::EqualEqual:
      return "==";
    case TokenKind::BangEqual:
      return "!=";
    case TokenKind::Less:
      return "<";
    case TokenKind::LessEqual:
      return "<=";
    case TokenKind::Greater:
      return ">";
    case TokenKind::GreaterEqual:
      return ">=";
    case TokenKind::Pipe:
      return "|>";
    case TokenKind::LeftParen:
      return "(";
    case TokenKind::RightParen:
      return ")";
    case TokenKind::LeftBrace:
      return "{";
    case TokenKind::RightBrace:
      return "}";
    case TokenKind::LeftBracket:
      return "[";
    case TokenKind::RightBracket:
      return "]";
    case TokenKind::Comma:
      return ",";
    case TokenKind::Colon:
      return ":";
    case TokenKind::Semicolon:
      return ";";
    case TokenKind::Arrow:
      return "->";
  }
  return "unknown";
}

}  // namespace kite

#pragma once

#include "common/source.hpp"

#include <cstdint>
#include <string>

namespace kite {

enum class TokenKind {
  Eof,
  Identifier,
  Integer,
  Float,
  String,
  True,
  False,
  Unit,
  Fn,
  Let,
  If,
  Else,
  While,
  For,
  Return,
  IntType,
  FloatType,
  BoolType,
  StringType,
  UnitType,
  And,
  Or,
  Not,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,
  Equal,
  EqualEqual,
  BangEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  Pipe,
  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,
  LeftBracket,
  RightBracket,
  Comma,
  Colon,
  Semicolon,
  Arrow,
};

struct Token {
  TokenKind kind{TokenKind::Eof};
  std::string lexeme;
  std::int64_t integer_value{0};
  double float_value{0.0};
  Span span{Span::synthetic()};
};

const char* token_kind_name(TokenKind kind);

}  // namespace kite

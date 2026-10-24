#include "doctest/doctest.h"

#include "lexer/lexer.hpp"
#include "lexer/token_dump.hpp"

TEST_CASE("empty source produces eof") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "", diagnostics);
  CHECK(tokens.size() == 1);
  CHECK(tokens.front().kind == kite::TokenKind::Eof);
}

TEST_CASE("lexer recognizes keywords literals and operators") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan(
      "<test>",
      "fn add(a: int, b: float) -> float { let ok = true and not false; return a + b; }",
      diagnostics);
  CHECK_FALSE(diagnostics.has_errors());
  CHECK(tokens[0].kind == kite::TokenKind::Fn);
  CHECK(tokens[1].kind == kite::TokenKind::Identifier);
  CHECK(tokens[3].kind == kite::TokenKind::Identifier);
  CHECK(tokens[4].kind == kite::TokenKind::Colon);
  CHECK(tokens[5].kind == kite::TokenKind::IntType);
  CHECK(tokens[9].kind == kite::TokenKind::FloatType);
  CHECK(tokens[12].kind == kite::TokenKind::FloatType);
  CHECK(tokens[13].kind == kite::TokenKind::LeftBrace);
  CHECK(tokens[16].kind == kite::TokenKind::Equal);
  CHECK(tokens[17].kind == kite::TokenKind::True);
  CHECK(tokens[18].kind == kite::TokenKind::And);
  CHECK(tokens[19].kind == kite::TokenKind::Not);
  CHECK(tokens[20].kind == kite::TokenKind::False);
  CHECK(tokens.back().kind == kite::TokenKind::Eof);
}

TEST_CASE("lexer decodes strings, floats, and comments") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan(
      "<test>",
      "// line\n/* block */ let text = \"a\\n\\\"b\"; let value = 1.25;",
      diagnostics);
  CHECK_FALSE(diagnostics.has_errors());
  CHECK(tokens[0].kind == kite::TokenKind::Let);
  CHECK(tokens[1].kind == kite::TokenKind::Identifier);
  CHECK(tokens[3].kind == kite::TokenKind::String);
  CHECK(tokens[3].lexeme == "a\n\"b");
  CHECK(tokens[5].kind == kite::TokenKind::Let);
  CHECK(tokens[8].kind == kite::TokenKind::Float);
  CHECK(tokens[8].float_value == doctest::Approx(1.25));
}

TEST_CASE("lexer reports lexical errors and continues") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "let x = @; let y = \"bad\\q\"; /* open", diagnostics);
  CHECK(tokens.back().kind == kite::TokenKind::Eof);
  CHECK(diagnostics.error_count() == 3);
  CHECK(diagnostics.all()[0].code == "L001");
  CHECK(diagnostics.all()[1].code == "L004");
  CHECK(diagnostics.all()[2].code == "L002");
}

TEST_CASE("token dump is deterministic") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("sample.kite", "let x = 42;", diagnostics);
  CHECK(kite::dump_tokens(tokens) ==
        "1:1 let `let`\n1:5 identifier `x`\n1:7 = `=`\n1:9 integer `42`\n1:11 ; `;`\n1:12 eof ``\n");
}

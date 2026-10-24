#include "doctest/doctest.h"

#include "lexer/lexer.hpp"
#include "parser/ast_printer.hpp"
#include "parser/parser.hpp"

TEST_CASE("parser parses functions and control flow") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan(
      "sample.kite",
      "fn add(a: int, b: int) -> int { if (a < b) { return b; } else { return a; } }",
      diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  REQUIRE(program != nullptr);
  CHECK_FALSE(diagnostics.has_errors());
  CHECK(program->declarations.size() == 1);
  CHECK(kite::print_ast(*program).find("FunctionDecl") != std::string::npos);
  CHECK(kite::print_ast(*program).find("IfStmt") != std::string::npos);
}

TEST_CASE("parser parses let assignment loops and return") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan(
      "sample.kite",
      "let i = 0; while (i < 3) { i = i + 1; } return;",
      diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  REQUIRE(program != nullptr);
  CHECK_FALSE(diagnostics.has_errors());
  CHECK(program->statements.size() == 3);
  const auto dump = kite::print_ast(*program);
  CHECK(dump.find("LetStmt") != std::string::npos);
  CHECK(dump.find("WhileStmt") != std::string::npos);
  CHECK(dump.find("AssignmentExpr") != std::string::npos);
  CHECK(dump.find("ReturnStmt") != std::string::npos);
}

TEST_CASE("parser recovers after malformed statement") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("sample.kite", "let x = ; let y = 2;", diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  REQUIRE(program != nullptr);
  CHECK(diagnostics.has_errors());
  CHECK(program->statements.size() == 2);
  CHECK(dynamic_cast<const kite::LetStmt*>(program->statements.back().get()) != nullptr);
}

TEST_CASE("parser reports missing delimiters") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("sample.kite", "if (true) { print(1);", diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  REQUIRE(program != nullptr);
  CHECK(diagnostics.has_errors());
  CHECK(diagnostics.all().back().code == "P002");
}

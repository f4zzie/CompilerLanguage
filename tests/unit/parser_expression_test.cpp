#include "doctest/doctest.h"

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

TEST_CASE("parser applies Pratt precedence") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "1 + 2 * 3", diagnostics);
  kite::Parser parser;
  auto expression = parser.parse_expression_for_test(tokens, diagnostics);
  REQUIRE(expression != nullptr);
  const auto* add = dynamic_cast<const kite::BinaryExpr*>(expression.get());
  REQUIRE(add != nullptr);
  CHECK(add->op == kite::BinaryOperator::Add);
  CHECK(dynamic_cast<const kite::LiteralExpr*>(add->left.get()) != nullptr);
  const auto* multiply = dynamic_cast<const kite::BinaryExpr*>(add->right.get());
  REQUIRE(multiply != nullptr);
  CHECK(multiply->op == kite::BinaryOperator::Multiply);
}

TEST_CASE("parser chains calls and indexes") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "value(1, 2)[0]", diagnostics);
  kite::Parser parser;
  auto expression = parser.parse_expression_for_test(tokens, diagnostics);
  REQUIRE(expression != nullptr);
  const auto* index = dynamic_cast<const kite::IndexExpr*>(expression.get());
  REQUIRE(index != nullptr);
  CHECK(dynamic_cast<const kite::CallExpr*>(index->object.get()) != nullptr);
  CHECK_FALSE(diagnostics.has_errors());
}

TEST_CASE("parser builds left-associative pipe expressions") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "5 |> double |> print", diagnostics);
  kite::Parser parser;
  auto expression = parser.parse_expression_for_test(tokens, diagnostics);
  REQUIRE(expression != nullptr);
  const auto* outer = dynamic_cast<const kite::PipeExpr*>(expression.get());
  REQUIRE(outer != nullptr);
  CHECK(outer->function_name == "print");
  const auto* inner = dynamic_cast<const kite::PipeExpr*>(outer->value.get());
  REQUIRE(inner != nullptr);
  CHECK(inner->function_name == "double");
}

TEST_CASE("parser rejects invalid assignment targets") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "1 = 2", diagnostics);
  kite::Parser parser;
  auto expression = parser.parse_expression_for_test(tokens, diagnostics);
  CHECK(expression != nullptr);
  CHECK(diagnostics.has_errors());
  REQUIRE(diagnostics.error_count() == 1);
  CHECK(diagnostics.all().front().code == "P003");
}

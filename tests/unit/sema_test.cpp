#include "doctest/doctest.h"

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "sema/sema.hpp"

namespace {

std::unique_ptr<kite::Program> parse_source(std::string_view source, kite::Diagnostics& diagnostics) {
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", source, diagnostics);
  kite::Parser parser;
  return parser.parse(tokens, diagnostics);
}

}  // namespace

TEST_CASE("semantic analyzer infers local let types") {
  kite::Diagnostics diagnostics;
  auto program = parse_source("let value = 1 + 2; print(value);", diagnostics);
  REQUIRE(program != nullptr);
  kite::SemanticAnalyzer analyzer;
  const auto result = analyzer.analyze(*program, diagnostics);
  CHECK(result.success);
  CHECK_FALSE(diagnostics.has_errors());
  const auto* declaration = dynamic_cast<const kite::LetStmt*>(program->statements.front().get());
  REQUIRE(declaration != nullptr);
  CHECK(declaration->inferred_type.kind == kite::TypeKind::Int);
}

TEST_CASE("semantic analyzer reports type mismatch and non-bool conditions") {
  kite::Diagnostics diagnostics;
  auto program = parse_source("let value: int = true; if (1) { print(value); }", diagnostics);
  REQUIRE(program != nullptr);
  kite::SemanticAnalyzer analyzer;
  const auto result = analyzer.analyze(*program, diagnostics);
  CHECK_FALSE(result.success);
  CHECK(diagnostics.error_count() >= 2);
  CHECK(diagnostics.all()[0].code == "T001");
  CHECK(diagnostics.all()[1].code == "T002");
}

TEST_CASE("semantic analyzer checks function calls and recursion") {
  kite::Diagnostics diagnostics;
  auto program = parse_source(
      "fn fact(n: int) -> int { if (n <= 1) { return 1; } return n * fact(n - 1); } "
      "print(fact(true));",
      diagnostics);
  REQUIRE(program != nullptr);
  kite::SemanticAnalyzer analyzer;
  const auto result = analyzer.analyze(*program, diagnostics);
  CHECK_FALSE(result.success);
  CHECK(diagnostics.error_count() == 1);
  CHECK(diagnostics.all().front().code == "T005");
}

TEST_CASE("semantic analyzer reports missing and mismatched returns") {
  kite::Diagnostics diagnostics;
  auto program = parse_source(
      "fn bad(flag: bool) -> int { if (flag) { return true; } }", diagnostics);
  REQUIRE(program != nullptr);
  kite::SemanticAnalyzer analyzer;
  const auto result = analyzer.analyze(*program, diagnostics);
  CHECK_FALSE(result.success);
  CHECK(diagnostics.error_count() >= 2);
  bool saw_return_mismatch = false;
  bool saw_missing_return = false;
  for (const auto& diagnostic : diagnostics.all()) {
    saw_return_mismatch = saw_return_mismatch || diagnostic.code == "T006";
    saw_missing_return = saw_missing_return || diagnostic.code == "T008";
  }
  CHECK(saw_return_mismatch);
  CHECK(saw_missing_return);
}

#include "doctest/doctest.h"

#include "codegen/code_generator.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "runtime/vm.hpp"
#include "sema/sema.hpp"

#include <sstream>

namespace {

std::string run_source(std::string_view source) {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", source, diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  kite::SemanticAnalyzer analyzer;
  analyzer.analyze(*program, diagnostics);
  REQUIRE_FALSE(diagnostics.has_errors());
  kite::CodeGenerator generator;
  const auto module = generator.generate(*program, diagnostics);
  std::ostringstream output;
  kite::Vm vm(kite::VmOptions{false, 1024, nullptr, &output});
  const auto result = vm.run(module);
  REQUIRE(result.success);
  return output.str();
}

}  // namespace

TEST_CASE("functions bind multiple parameters and return a value") {
  CHECK(run_source("fn add(a: int, b: int) -> int { return a + b; } print(add(2, 3));") == "5\n");
}

TEST_CASE("function calls preserve caller execution") {
  CHECK(run_source("fn twice(n: int) -> int { return n * 2; } let x = 4; print(twice(x) + x);") == "12\n");
}

TEST_CASE("wrong function arity is a semantic error") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "fn add(a: int, b: int) -> int { return a + b; } add(1);", diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  kite::SemanticAnalyzer analyzer;
  analyzer.analyze(*program, diagnostics);
  CHECK(diagnostics.has_errors());
  CHECK(diagnostics.all().front().code == "T004");
}

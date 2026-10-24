#include "doctest/doctest.h"

#include "codegen/code_generator.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "runtime/vm.hpp"
#include "sema/sema.hpp"

#include <sstream>

TEST_CASE("pipe applies functions from left to right") {
  const std::string source =
      "fn double(n: int) -> int { return n * 2; } 5 |> double |> print;";
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
  CHECK(output.str() == "10\n");
}

TEST_CASE("pipe rejects an unresolved target") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "1 |> missing;", diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  kite::SemanticAnalyzer analyzer;
  analyzer.analyze(*program, diagnostics);
  CHECK(diagnostics.has_errors());
  CHECK(diagnostics.all().front().code == "S002");
}

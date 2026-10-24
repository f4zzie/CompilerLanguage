#include "doctest/doctest.h"

#include "codegen/code_generator.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "runtime/vm.hpp"
#include "sema/sema.hpp"

#include <sstream>

TEST_CASE("arrays, floats, and for loops execute") {
  const std::string source =
      "let values: int[] = [1, 2, 3]; let total = 0; "
      "for (let i = 0; i < len(values); i = i + 1) { total = total + values[i]; } "
      "print(total); print(1.5 + 2.0);";
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
  CHECK(output.str() == "6\n3.5\n");
}

TEST_CASE("array indexing reports an out of range runtime error") {
  const std::string source = "let values: int[] = [1]; print(values[2]);";
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
  const auto result = kite::Vm{}.run(module);
  CHECK_FALSE(result.success);
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == "R003");
}

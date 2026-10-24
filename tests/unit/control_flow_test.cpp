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
  kite::CodeGenerator generator;
  const auto module = generator.generate(*program, diagnostics);
  std::ostringstream output;
  kite::Vm vm(kite::VmOptions{false, 1024, nullptr, &output});
  const auto result = vm.run(module);
  REQUIRE(result.success);
  REQUIRE_FALSE(diagnostics.has_errors());
  return output.str();
}

}  // namespace

TEST_CASE("if else selects the correct branch") {
  CHECK(run_source("if (1 < 2) { print(\"yes\"); } else { print(\"no\"); }") == "yes\n");
}

TEST_CASE("while repeats until its condition becomes false") {
  CHECK(run_source("let i = 0; while (i < 3) { print(i); i = i + 1; }") == "0\n1\n2\n");
}

TEST_CASE("boolean logic short circuits") {
  CHECK(run_source("if (false and (1 / 0 == 0)) { print(1); } else { print(2); }") == "2\n");
}

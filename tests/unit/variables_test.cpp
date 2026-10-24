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

TEST_CASE("variables support nested shadowing") {
  CHECK(run_source("let x = 1; { let x = 2; print(x); } print(x);") == "2\n1\n");
}

TEST_CASE("variables support assignment") {
  CHECK(run_source("let x = 1; x = x + 2; print(x);") == "3\n");
}

#include "doctest/doctest.h"

#include "codegen/code_generator.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "runtime/vm.hpp"
#include "sema/sema.hpp"

#include <sstream>

namespace {

std::string run_source(std::string_view source, bool* success = nullptr) {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", source, diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  kite::SemanticAnalyzer analyzer;
  analyzer.analyze(*program, diagnostics);
  if (diagnostics.has_errors()) {
    if (success != nullptr) *success = false;
    return {};
  }
  kite::CodeGenerator generator;
  const auto module = generator.generate(*program, diagnostics);
  std::ostringstream output;
  kite::Vm vm(kite::VmOptions{false, 1024, nullptr, &output});
  const auto result = vm.run(module);
  if (success != nullptr) *success = result.success;
  return output.str();
}

}  // namespace

TEST_CASE("recursive factorial and fibonacci produce exact values") {
  CHECK(run_source(
            "fn fact(n: int) -> int { if (n <= 1) { return 1; } return n * fact(n - 1); } "
            "fn fib(n: int) -> int { if (n < 2) { return n; } return fib(n - 1) + fib(n - 2); } "
            "print(fact(5)); print(fib(6));") ==
        "120\n8\n");
}

TEST_CASE("missing return is rejected") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "fn bad(flag: bool) -> int { if (flag) { return 1; } }", diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  kite::SemanticAnalyzer analyzer;
  analyzer.analyze(*program, diagnostics);
  CHECK(diagnostics.has_errors());
  bool saw_missing = false;
  for (const auto& diagnostic : diagnostics.all()) saw_missing = saw_missing || diagnostic.code == "T008";
  CHECK(saw_missing);
}

TEST_CASE("recursive calls have a bounded call stack") {
  bool success = true;
  run_source("fn loop(n: int) -> int { return loop(n + 1); } print(loop(0));", &success);
  CHECK_FALSE(success);
}

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

TEST_CASE("strings concatenate and len reports byte length") {
  CHECK(run_source("let name = \"Kite\"; print(\"Hello, \" + name); print(len(name));") ==
        "Hello, Kite\n4\n");
}

TEST_CASE("conversion builtins produce strings and integers") {
  CHECK(run_source("print(to_string(42)); print(to_int(\"17\"));") == "42\n17\n");
}

TEST_CASE("to_int rejects invalid input at runtime") {
  bool success = true;
  run_source("print(to_int(\"kite\"));", &success);
  CHECK_FALSE(success);
}

#include "doctest/doctest.h"

#include "codegen/code_generator.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "sema/sema.hpp"

namespace {

kite::BytecodeModule compile_source(std::string_view source, kite::Diagnostics& diagnostics) {
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", source, diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  kite::SemanticAnalyzer analyzer;
  analyzer.analyze(*program, diagnostics);
  kite::CodeGenerator generator;
  return generator.generate(*program, diagnostics);
}

}  // namespace

TEST_CASE("code generator emits constants arithmetic and halt") {
  kite::Diagnostics diagnostics;
  const auto module = compile_source("print(1 + 2);", diagnostics);
  CHECK_FALSE(diagnostics.has_errors());
  CHECK(module.instructions.size() >= 5);
  CHECK(module.instructions[0].opcode == kite::OpCode::Constant);
  CHECK(module.instructions[1].opcode == kite::OpCode::Constant);
  CHECK(module.instructions[2].opcode == kite::OpCode::Add);
  CHECK(module.instructions.back().opcode == kite::OpCode::Halt);
}

TEST_CASE("code generator emits function metadata and calls") {
  kite::Diagnostics diagnostics;
  const auto module = compile_source(
      "fn add(a: int, b: int) -> int { return a + b; } print(add(1, 2));", diagnostics);
  CHECK_FALSE(diagnostics.has_errors());
  CHECK(module.functions.size() >= 2);
  CHECK(module.functions[0].name == "__main");
  bool saw_add = false;
  for (const auto& function : module.functions) {
    saw_add = saw_add || function.name == "add";
  }
  CHECK(saw_add);
  bool saw_call = false;
  for (const auto& instruction : module.instructions) {
    saw_call = saw_call || instruction.opcode == kite::OpCode::Call;
  }
  CHECK(saw_call);
}

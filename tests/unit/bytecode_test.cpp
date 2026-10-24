#include "doctest/doctest.h"

#include "codegen/bytecode.hpp"

TEST_CASE("bytecode disassembler prints instruction indices and operands") {
  kite::BytecodeModule module;
  module.constants.emplace_back(std::int64_t{42});
  module.instructions.push_back(kite::Instruction{kite::OpCode::Constant, 0, kite::Span::synthetic()});
  module.instructions.push_back(kite::Instruction{kite::OpCode::Halt, 0, kite::Span::synthetic()});
  CHECK(kite::disassemble(module) == "0\tCONSTANT\t0\n1\tHALT\t0\n");
}

TEST_CASE("bytecode module keeps function metadata") {
  kite::BytecodeModule module;
  module.functions.push_back(kite::FunctionInfo{"__main", 0, 0, 2});
  CHECK(module.functions.front().name == "__main");
  CHECK(module.functions.front().entry == 0);
  CHECK(module.functions.front().local_count == 2);
}

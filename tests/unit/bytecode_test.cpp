#include "doctest/doctest.h"

#include "codegen/bytecode.hpp"

#include <sstream>

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

TEST_CASE("bytecode binary format round trips constants functions and code") {
  kite::BytecodeModule module;
  module.constants = {std::int64_t{42}, 2.5, true, std::string("Kite")};
  module.functions.push_back(kite::FunctionInfo{"__main", 0, 0, 0});
  module.instructions = {
      {kite::OpCode::Constant, 0, kite::Span::synthetic()},
      {kite::OpCode::Pop, 0, kite::Span::synthetic()},
      {kite::OpCode::Halt, 0, kite::Span::synthetic()},
  };
  std::stringstream bytes;
  std::string error;
  REQUIRE(kite::write_bytecode(bytes, module, error));
  const auto decoded = kite::read_bytecode(bytes, error);
  REQUIRE(decoded.has_value());
  CHECK(decoded->constants == module.constants);
  CHECK(decoded->functions.front().name == "__main");
  CHECK(decoded->instructions.size() == module.instructions.size());
  CHECK(decoded->instructions[0].opcode == kite::OpCode::Constant);
  CHECK(decoded->instructions[0].operand == 0);
}

TEST_CASE("bytecode loader rejects truncated data") {
  std::stringstream bytes("KBC\0\1", std::ios::in | std::ios::out | std::ios::binary);
  std::string error;
  CHECK_FALSE(kite::read_bytecode(bytes, error).has_value());
  CHECK(error == "truncated bytecode header");
}

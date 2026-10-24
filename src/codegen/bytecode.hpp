#pragma once

#include "common/source.hpp"

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace kite {

enum class OpCode {
  Constant,
  Unit,
  Add,
  Subtract,
  Multiply,
  Divide,
  Modulo,
  Negate,
  Not,
  Equal,
  Less,
  Greater,
  Jump,
  JumpIfFalse,
  LoadLocal,
  StoreLocal,
  LoadGlobal,
  StoreGlobal,
  Call,
  Return,
  Pop,
  Halt,
};

struct Instruction {
  OpCode opcode{OpCode::Halt};
  std::int32_t operand{0};
  Span span{Span::synthetic()};
};

using Constant = std::variant<std::int64_t, double, bool, std::string>;

struct FunctionInfo {
  std::string name;
  std::size_t entry{0};
  std::size_t arity{0};
  std::size_t local_count{0};
};

struct BytecodeModule {
  std::vector<Constant> constants;
  std::vector<Instruction> instructions;
  std::vector<FunctionInfo> functions;
};

const char* opcode_name(OpCode opcode);
std::string disassemble(const BytecodeModule& module);

}  // namespace kite

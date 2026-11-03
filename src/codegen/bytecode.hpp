#pragma once

#include "common/source.hpp"

#include <cstdint>
#include <istream>
#include <optional>
#include <ostream>
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
  MakeArray,
  Index,
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
  std::size_t global_count{0};
};

const char* opcode_name(OpCode opcode);
std::string disassemble(const BytecodeModule& module);
bool write_bytecode(std::ostream& output, const BytecodeModule& module, std::string& error);
std::optional<BytecodeModule> read_bytecode(std::istream& input, std::string& error);
bool write_bytecode_file(const std::string& path, const BytecodeModule& module, std::string& error);
std::optional<BytecodeModule> read_bytecode_file(const std::string& path, std::string& error);

}  // namespace kite

#include "codegen/bytecode.hpp"

#include <sstream>

namespace kite {

const char* opcode_name(OpCode opcode) {
  switch (opcode) {
    case OpCode::Constant:
      return "CONSTANT";
    case OpCode::Unit:
      return "UNIT";
    case OpCode::Add:
      return "ADD";
    case OpCode::Subtract:
      return "SUBTRACT";
    case OpCode::Multiply:
      return "MULTIPLY";
    case OpCode::Divide:
      return "DIVIDE";
    case OpCode::Modulo:
      return "MODULO";
    case OpCode::Negate:
      return "NEGATE";
    case OpCode::Not:
      return "NOT";
    case OpCode::Equal:
      return "EQUAL";
    case OpCode::Less:
      return "LESS";
    case OpCode::Greater:
      return "GREATER";
    case OpCode::Jump:
      return "JUMP";
    case OpCode::JumpIfFalse:
      return "JUMP_IF_FALSE";
    case OpCode::LoadLocal:
      return "LOAD_LOCAL";
    case OpCode::StoreLocal:
      return "STORE_LOCAL";
    case OpCode::LoadGlobal:
      return "LOAD_GLOBAL";
    case OpCode::StoreGlobal:
      return "STORE_GLOBAL";
    case OpCode::Call:
      return "CALL";
    case OpCode::MakeArray:
      return "MAKE_ARRAY";
    case OpCode::Index:
      return "INDEX";
    case OpCode::Return:
      return "RETURN";
    case OpCode::Pop:
      return "POP";
    case OpCode::Halt:
      return "HALT";
  }
  return "UNKNOWN";
}

std::string disassemble(const BytecodeModule& module) {
  std::ostringstream output;
  for (std::size_t index = 0; index < module.instructions.size(); ++index) {
    const auto& instruction = module.instructions[index];
    output << index << "\t" << opcode_name(instruction.opcode) << "\t" << instruction.operand << '\n';
  }
  return output.str();
}

}  // namespace kite

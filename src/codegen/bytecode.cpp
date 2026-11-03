#include "codegen/bytecode.hpp"

#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>
#include <type_traits>
#include <utility>

namespace kite {

namespace {

constexpr char kMagic[] = {'K', 'B', 'C', '\0'};
constexpr std::uint32_t kFormatVersion = 1;
constexpr std::uint32_t kMaximumCount = 1'000'000;
constexpr std::uint32_t kMaximumStringSize = 16 * 1024 * 1024;

bool write_bytes(std::ostream& output, const void* data, std::size_t size) {
  if (size == 0) return output.good();
  output.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
  return output.good();
}

bool read_bytes(std::istream& input, void* data, std::size_t size) {
  input.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
  return input.gcount() == static_cast<std::streamsize>(size);
}

bool write_u8(std::ostream& output, std::uint8_t value) { return write_bytes(output, &value, sizeof(value)); }

bool read_u8(std::istream& input, std::uint8_t& value) { return read_bytes(input, &value, sizeof(value)); }

bool write_u32(std::ostream& output, std::uint32_t value) {
  const std::uint8_t bytes[] = {static_cast<std::uint8_t>(value & 0xffU),
                                static_cast<std::uint8_t>((value >> 8U) & 0xffU),
                                static_cast<std::uint8_t>((value >> 16U) & 0xffU),
                                static_cast<std::uint8_t>((value >> 24U) & 0xffU)};
  return write_bytes(output, bytes, sizeof(bytes));
}

bool read_u32(std::istream& input, std::uint32_t& value) {
  std::uint8_t bytes[4]{};
  if (!read_bytes(input, bytes, sizeof(bytes))) return false;
  value = static_cast<std::uint32_t>(bytes[0]) | (static_cast<std::uint32_t>(bytes[1]) << 8U) |
          (static_cast<std::uint32_t>(bytes[2]) << 16U) | (static_cast<std::uint32_t>(bytes[3]) << 24U);
  return true;
}

bool write_i32(std::ostream& output, std::int32_t value) {
  return write_u32(output, static_cast<std::uint32_t>(value));
}

bool read_i32(std::istream& input, std::int32_t& value) {
  std::uint32_t raw = 0;
  if (!read_u32(input, raw)) return false;
  value = static_cast<std::int32_t>(raw);
  return true;
}

bool write_u64(std::ostream& output, std::uint64_t value) {
  const std::uint8_t bytes[] = {
      static_cast<std::uint8_t>(value & 0xffU), static_cast<std::uint8_t>((value >> 8U) & 0xffU),
      static_cast<std::uint8_t>((value >> 16U) & 0xffU), static_cast<std::uint8_t>((value >> 24U) & 0xffU),
      static_cast<std::uint8_t>((value >> 32U) & 0xffU), static_cast<std::uint8_t>((value >> 40U) & 0xffU),
      static_cast<std::uint8_t>((value >> 48U) & 0xffU), static_cast<std::uint8_t>((value >> 56U) & 0xffU)};
  return write_bytes(output, bytes, sizeof(bytes));
}

bool read_u64(std::istream& input, std::uint64_t& value) {
  std::uint8_t bytes[8]{};
  if (!read_bytes(input, bytes, sizeof(bytes))) return false;
  value = static_cast<std::uint64_t>(bytes[0]) | (static_cast<std::uint64_t>(bytes[1]) << 8U) |
          (static_cast<std::uint64_t>(bytes[2]) << 16U) | (static_cast<std::uint64_t>(bytes[3]) << 24U) |
          (static_cast<std::uint64_t>(bytes[4]) << 32U) | (static_cast<std::uint64_t>(bytes[5]) << 40U) |
          (static_cast<std::uint64_t>(bytes[6]) << 48U) | (static_cast<std::uint64_t>(bytes[7]) << 56U);
  return true;
}

bool write_string(std::ostream& output, const std::string& value) {
  return value.size() <= std::numeric_limits<std::uint32_t>::max() &&
         write_u32(output, static_cast<std::uint32_t>(value.size())) &&
         write_bytes(output, value.data(), value.size());
}

bool read_string(std::istream& input, std::string& value) {
  std::uint32_t size = 0;
  if (!read_u32(input, size) || size > kMaximumStringSize) return false;
  value.resize(size);
  return read_bytes(input, value.data(), size);
}

bool count_fits(std::size_t value) { return value <= kMaximumCount; }

void set_error(std::string& error, std::string message) { error = std::move(message); }

}  // namespace

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

bool write_bytecode(std::ostream& output, const BytecodeModule& module, std::string& error) {
  if (!count_fits(module.constants.size()) || !count_fits(module.functions.size()) ||
      !count_fits(module.instructions.size()) || module.global_count > kMaximumCount) {
    set_error(error, "bytecode module is too large");
    return false;
  }
  if (!write_bytes(output, kMagic, sizeof(kMagic)) || !write_u32(output, kFormatVersion) ||
      !write_u32(output, static_cast<std::uint32_t>(module.constants.size()))) {
    set_error(error, "could not write bytecode header");
    return false;
  }
  for (const auto& constant : module.constants) {
    bool written = std::visit(
        [&](const auto& value) {
          using ValueType = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<ValueType, std::int64_t>) {
            return write_u8(output, 0) && write_u64(output, static_cast<std::uint64_t>(value));
          } else if constexpr (std::is_same_v<ValueType, double>) {
            std::uint64_t bits = 0;
            static_assert(sizeof(bits) == sizeof(value));
            std::memcpy(&bits, &value, sizeof(bits));
            return write_u8(output, 1) && write_u64(output, bits);
          } else if constexpr (std::is_same_v<ValueType, bool>) {
            return write_u8(output, 2) && write_u8(output, value ? 1 : 0);
          } else {
            return write_u8(output, 3) && write_string(output, value);
          }
        },
        constant);
    if (!written) {
      set_error(error, "could not write constant pool");
      return false;
    }
  }
  if (!write_u32(output, static_cast<std::uint32_t>(module.functions.size()))) {
    set_error(error, "could not write function table");
    return false;
  }
  for (const auto& function : module.functions) {
    if (!count_fits(function.entry) || !count_fits(function.arity) || !count_fits(function.local_count) ||
        !write_string(output, function.name) || !write_u32(output, static_cast<std::uint32_t>(function.entry)) ||
        !write_u32(output, static_cast<std::uint32_t>(function.arity)) ||
        !write_u32(output, static_cast<std::uint32_t>(function.local_count))) {
      set_error(error, "could not write function table");
      return false;
    }
  }
  if (!write_u32(output, static_cast<std::uint32_t>(module.global_count)) ||
      !write_u32(output, static_cast<std::uint32_t>(module.instructions.size()))) {
    set_error(error, "could not write bytecode metadata");
    return false;
  }
  for (const auto& instruction : module.instructions) {
    const auto opcode = static_cast<std::uint8_t>(instruction.opcode);
    if (opcode > static_cast<std::uint8_t>(OpCode::Halt) || !write_u8(output, opcode) ||
        !write_i32(output, instruction.operand)) {
      set_error(error, "could not write instruction stream");
      return false;
    }
  }
  if (!output.good()) {
    set_error(error, "could not finish bytecode file");
    return false;
  }
  return true;
}

std::optional<BytecodeModule> read_bytecode(std::istream& input, std::string& error) {
  char magic[sizeof(kMagic)]{};
  if (!read_bytes(input, magic, sizeof(magic))) {
    set_error(error, "truncated bytecode header");
    return std::nullopt;
  }
  if (std::memcmp(magic, kMagic, sizeof(kMagic)) != 0) {
    set_error(error, "invalid bytecode magic");
    return std::nullopt;
  }
  std::uint32_t version = 0;
  if (!read_u32(input, version)) {
    set_error(error, "truncated bytecode version");
    return std::nullopt;
  }
  if (version != kFormatVersion) {
    set_error(error, "unsupported bytecode version");
    return std::nullopt;
  }
  BytecodeModule module;
  std::uint32_t count = 0;
  if (!read_u32(input, count) || count > kMaximumCount) {
    set_error(error, "invalid constant count");
    return std::nullopt;
  }
  module.constants.reserve(count);
  for (std::uint32_t index = 0; index < count; ++index) {
    std::uint8_t tag = 0;
    if (!read_u8(input, tag)) {
      set_error(error, "truncated constant pool");
      return std::nullopt;
    }
    if (tag == 0) {
      std::uint64_t value = 0;
      if (!read_u64(input, value)) {
        set_error(error, "truncated integer constant");
        return std::nullopt;
      }
      module.constants.emplace_back(static_cast<std::int64_t>(value));
    } else if (tag == 1) {
      std::uint64_t bits = 0;
      if (!read_u64(input, bits)) {
        set_error(error, "truncated float constant");
        return std::nullopt;
      }
      double value = 0.0;
      static_assert(sizeof(bits) == sizeof(value));
      std::memcpy(&value, &bits, sizeof(value));
      module.constants.emplace_back(value);
    } else if (tag == 2) {
      std::uint8_t value = 0;
      if (!read_u8(input, value) || value > 1) {
        set_error(error, "invalid boolean constant");
        return std::nullopt;
      }
      module.constants.emplace_back(value != 0);
    } else if (tag == 3) {
      std::string value;
      if (!read_string(input, value)) {
        set_error(error, "truncated string constant");
        return std::nullopt;
      }
      module.constants.emplace_back(std::move(value));
    } else {
      set_error(error, "invalid constant tag");
      return std::nullopt;
    }
  }
  if (!read_u32(input, count) || count > kMaximumCount) {
    set_error(error, "invalid function count");
    return std::nullopt;
  }
  module.functions.reserve(count);
  for (std::uint32_t index = 0; index < count; ++index) {
    FunctionInfo function;
    std::uint32_t entry = 0;
    std::uint32_t arity = 0;
    std::uint32_t locals = 0;
    if (!read_string(input, function.name) || !read_u32(input, entry) || !read_u32(input, arity) ||
        !read_u32(input, locals)) {
      set_error(error, "truncated function table");
      return std::nullopt;
    }
    function.entry = entry;
    function.arity = arity;
    function.local_count = locals;
    module.functions.push_back(std::move(function));
  }
  std::uint32_t globals = 0;
  std::uint32_t instructions = 0;
  if (!read_u32(input, globals) || globals > kMaximumCount || !read_u32(input, instructions) ||
      instructions > kMaximumCount) {
    set_error(error, "invalid bytecode code counts");
    return std::nullopt;
  }
  module.global_count = globals;
  module.instructions.reserve(instructions);
  for (std::uint32_t index = 0; index < instructions; ++index) {
    std::uint8_t opcode = 0;
    std::int32_t operand = 0;
    if (!read_u8(input, opcode) || !read_i32(input, operand) || opcode > static_cast<std::uint8_t>(OpCode::Halt)) {
      set_error(error, "invalid or truncated instruction stream");
      return std::nullopt;
    }
    module.instructions.push_back(Instruction{static_cast<OpCode>(opcode), operand, Span::synthetic()});
  }
  if (module.functions.empty() || module.functions.front().name != "__main") {
    set_error(error, "missing __main function");
    return std::nullopt;
  }
  for (const auto& function : module.functions) {
    if (function.entry >= module.instructions.size()) {
      set_error(error, "function entry is outside code");
      return std::nullopt;
    }
  }
  for (const auto& instruction : module.instructions) {
    if (instruction.opcode == OpCode::Constant &&
        (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= module.constants.size())) {
      set_error(error, "constant index is outside the constant pool");
      return std::nullopt;
    }
    if ((instruction.opcode == OpCode::Jump || instruction.opcode == OpCode::JumpIfFalse) &&
        (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= module.instructions.size())) {
      set_error(error, "jump target is outside code");
      return std::nullopt;
    }
    if (instruction.opcode == OpCode::Call &&
        (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= module.functions.size())) {
      set_error(error, "function index is outside the function table");
      return std::nullopt;
    }
  }
  char trailing = 0;
  if (input.get(trailing)) {
    set_error(error, "trailing data after bytecode module");
    return std::nullopt;
  }
  return module;
}

bool write_bytecode_file(const std::string& path, const BytecodeModule& module, std::string& error) {
  std::ofstream output(path, std::ios::binary);
  if (!output) {
    set_error(error, "cannot write bytecode file");
    return false;
  }
  return write_bytecode(output, module, error);
}

std::optional<BytecodeModule> read_bytecode_file(const std::string& path, std::string& error) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    set_error(error, "cannot open bytecode file");
    return std::nullopt;
  }
  return read_bytecode(input, error);
}

}  // namespace kite

#include "runtime/vm.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <type_traits>
#include <utility>

namespace kite {
namespace {

struct Frame {
  std::size_t function_index{0};
  std::size_t ip{0};
  std::size_t return_ip{0};
  std::vector<Value> locals;
};

bool is_number(const Value& value) {
  return std::holds_alternative<std::int64_t>(value) || std::holds_alternative<double>(value);
}

bool truthy(const Value& value) {
  return std::holds_alternative<bool>(value) && std::get<bool>(value);
}

bool values_equal(const Value& left, const Value& right) {
  if (left.index() != right.index()) {
    return false;
  }
  return std::visit(
      [](const auto& a, const auto& b) {
        using Left = std::decay_t<decltype(a)>;
        using Right = std::decay_t<decltype(b)>;
        if constexpr (std::is_same_v<Left, Right>) {
          return a == b;
        } else {
          return false;
        }
      },
      left, right);
}

bool add_overflow(std::int64_t left, std::int64_t right, std::int64_t& result) {
  if ((right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) ||
      (right < 0 && left < std::numeric_limits<std::int64_t>::min() - right)) {
    return true;
  }
  result = left + right;
  return false;
}

bool subtract_overflow(std::int64_t left, std::int64_t right, std::int64_t& result) {
  if ((right < 0 && left > std::numeric_limits<std::int64_t>::max() + right) ||
      (right > 0 && left < std::numeric_limits<std::int64_t>::min() + right)) {
    return true;
  }
  result = left - right;
  return false;
}

bool multiply_overflow(std::int64_t left, std::int64_t right, std::int64_t& result) {
  if (left == 0 || right == 0) {
    result = 0;
    return false;
  }
  if (left == -1 && right == std::numeric_limits<std::int64_t>::min()) return true;
  if (right == -1 && left == std::numeric_limits<std::int64_t>::min()) return true;
  if (left > 0) {
    if (right > 0 && left > std::numeric_limits<std::int64_t>::max() / right) return true;
    if (right < 0 && right < std::numeric_limits<std::int64_t>::min() / left) return true;
  } else {
    if (right > 0 && left < std::numeric_limits<std::int64_t>::min() / right) return true;
    if (right < 0 && left < std::numeric_limits<std::int64_t>::max() / right) return true;
  }
  result = left * right;
  return false;
}

std::string stack_text(const std::vector<Value>& stack) {
  std::ostringstream output;
  output << '[';
  for (std::size_t index = 0; index < stack.size(); ++index) {
    if (index != 0) {
      output << ", ";
    }
    output << value_to_string(stack[index]);
  }
  output << ']';
  return output.str();
}

}  // namespace

std::string value_to_string(const Value& value) {
  return std::visit(
      [](const auto& item) {
        using Item = std::decay_t<decltype(item)>;
        std::ostringstream output;
        if constexpr (std::is_same_v<Item, UnitValue>) {
          return std::string("unit");
        } else if constexpr (std::is_same_v<Item, std::shared_ptr<RuntimeArray>>) {
          std::ostringstream array;
          array << '[';
          if (item != nullptr) {
            for (std::size_t index = 0; index < item->elements.size(); ++index) {
              if (index != 0) array << ", ";
              array << value_to_string(item->elements[index]);
            }
          }
          array << ']';
          return array.str();
        } else if constexpr (std::is_same_v<Item, bool>) {
          return item ? std::string("true") : std::string("false");
        } else {
          output << item;
          return output.str();
        }
      },
      value);
}

VmResult Vm::run(const BytecodeModule& module) {
  stack_.clear();
  std::vector<Value> globals(module.global_count, UnitValue{});
  std::vector<Frame> frames;
  if (module.functions.empty() || module.functions[0].name != "__main") {
    return VmResult{false, UnitValue{}, RuntimeError{"R006", "missing __main function", Span::synthetic()}};
  }
  frames.push_back(Frame{0, module.functions[0].entry, 0,
                         std::vector<Value>(module.functions[0].local_count, UnitValue{})});
  std::ostream& trace = options_.trace_stream == nullptr ? std::cerr : *options_.trace_stream;
  std::ostream& output = options_.output_stream == nullptr ? std::cout : *options_.output_stream;

  auto fail = [](std::string code, std::string message, Span span) {
    return VmResult{false, UnitValue{}, RuntimeError{std::move(code), std::move(message), std::move(span)}};
  };
  auto pop = [&](Span span) -> std::optional<Value> {
    if (stack_.empty()) {
      return std::nullopt;
    }
    Value value = stack_.back();
    stack_.pop_back();
    (void)span;
    return value;
  };

  while (!frames.empty()) {
    Frame& frame = frames.back();
    if (frame.ip >= module.instructions.size()) {
      return fail("R006", "instruction pointer is outside bytecode", Span::synthetic());
    }
    const auto instruction = module.instructions[frame.ip++];
    if (options_.trace) {
      trace << frame.ip - 1 << ' ' << opcode_name(instruction.opcode) << ' ' << stack_text(stack_) << '\n';
    }
    switch (instruction.opcode) {
      case OpCode::Constant:
        if (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= module.constants.size()) {
          return fail("R006", "constant index is outside the constant pool", instruction.span);
        }
        std::visit([&](const auto& constant) { stack_.emplace_back(constant); },
                   module.constants[static_cast<std::size_t>(instruction.operand)]);
        break;
      case OpCode::Unit:
        stack_.emplace_back(UnitValue{});
        break;
      case OpCode::Add:
      case OpCode::Subtract:
      case OpCode::Multiply:
      case OpCode::Divide:
      case OpCode::Modulo: {
        const auto right = pop(instruction.span);
        const auto left = pop(instruction.span);
        if (!left.has_value() || !right.has_value()) {
          return fail("R006", "operand stack underflow", instruction.span);
        }
        if (instruction.opcode == OpCode::Add && std::holds_alternative<std::string>(*left) &&
            std::holds_alternative<std::string>(*right)) {
          stack_.emplace_back(std::get<std::string>(*left) + std::get<std::string>(*right));
          break;
        }
        if (!is_number(*left) || !is_number(*right)) {
          return fail("R006", "arithmetic operands must be numeric", instruction.span);
        }
        if (std::holds_alternative<double>(*left) || std::holds_alternative<double>(*right)) {
          const double a = std::holds_alternative<double>(*left) ? std::get<double>(*left)
                                                                  : static_cast<double>(std::get<std::int64_t>(*left));
          const double b = std::holds_alternative<double>(*right) ? std::get<double>(*right)
                                                                   : static_cast<double>(std::get<std::int64_t>(*right));
          if ((instruction.opcode == OpCode::Divide || instruction.opcode == OpCode::Modulo) && b == 0.0) {
            return fail("R001", "division by zero", instruction.span);
          }
          double result = 0.0;
          if (instruction.opcode == OpCode::Add) result = a + b;
          if (instruction.opcode == OpCode::Subtract) result = a - b;
          if (instruction.opcode == OpCode::Multiply) result = a * b;
          if (instruction.opcode == OpCode::Divide) result = a / b;
          if (instruction.opcode == OpCode::Modulo) result = std::fmod(a, b);
          stack_.emplace_back(result);
        } else {
          const auto a = std::get<std::int64_t>(*left);
          const auto b = std::get<std::int64_t>(*right);
          if ((instruction.opcode == OpCode::Divide || instruction.opcode == OpCode::Modulo) && b == 0) {
            return fail("R001", "division by zero", instruction.span);
          }
          if ((instruction.opcode == OpCode::Divide || instruction.opcode == OpCode::Modulo) &&
              a == std::numeric_limits<std::int64_t>::min() && b == -1) {
            return fail("R002", "integer overflow", instruction.span);
          }
          std::int64_t result = 0;
          if (instruction.opcode == OpCode::Add && add_overflow(a, b, result)) return fail("R002", "integer overflow", instruction.span);
          if (instruction.opcode == OpCode::Subtract && subtract_overflow(a, b, result)) return fail("R002", "integer overflow", instruction.span);
          if (instruction.opcode == OpCode::Multiply && multiply_overflow(a, b, result)) return fail("R002", "integer overflow", instruction.span);
          if (instruction.opcode == OpCode::Divide) result = a / b;
          if (instruction.opcode == OpCode::Modulo) result = a % b;
          stack_.emplace_back(result);
        }
        break;
      }
      case OpCode::Negate: {
        const auto value = pop(instruction.span);
        if (!value.has_value()) return fail("R006", "operand stack underflow", instruction.span);
        if (std::holds_alternative<std::int64_t>(*value)) {
          if (std::get<std::int64_t>(*value) == std::numeric_limits<std::int64_t>::min()) return fail("R002", "integer overflow", instruction.span);
          stack_.emplace_back(-std::get<std::int64_t>(*value));
        } else if (std::holds_alternative<double>(*value)) {
          stack_.emplace_back(-std::get<double>(*value));
        } else return fail("R006", "negation operand must be numeric", instruction.span);
        break;
      }
      case OpCode::Not: {
        const auto value = pop(instruction.span);
        if (!value.has_value() || !std::holds_alternative<bool>(*value)) return fail("R006", "not operand must be bool", instruction.span);
        stack_.emplace_back(!std::get<bool>(*value));
        break;
      }
      case OpCode::Equal: {
        const auto right = pop(instruction.span);
        const auto left = pop(instruction.span);
        if (!left.has_value() || !right.has_value()) return fail("R006", "operand stack underflow", instruction.span);
        stack_.emplace_back(values_equal(*left, *right));
        break;
      }
      case OpCode::Less:
      case OpCode::Greater: {
        const auto right = pop(instruction.span);
        const auto left = pop(instruction.span);
        if (!left.has_value() || !right.has_value() || !is_number(*left) || !is_number(*right)) return fail("R006", "comparison operands must be numeric", instruction.span);
        const double a = std::holds_alternative<double>(*left) ? std::get<double>(*left) : static_cast<double>(std::get<std::int64_t>(*left));
        const double b = std::holds_alternative<double>(*right) ? std::get<double>(*right) : static_cast<double>(std::get<std::int64_t>(*right));
        stack_.emplace_back(instruction.opcode == OpCode::Less ? a < b : a > b);
        break;
      }
      case OpCode::Jump:
        if (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= module.instructions.size()) return fail("R006", "jump target is outside bytecode", instruction.span);
        frame.ip = static_cast<std::size_t>(instruction.operand);
        break;
      case OpCode::JumpIfFalse: {
        const auto value = pop(instruction.span);
        if (!value.has_value() || !std::holds_alternative<bool>(*value)) return fail("R006", "conditional value must be bool", instruction.span);
        if (!truthy(*value)) frame.ip = static_cast<std::size_t>(instruction.operand);
        break;
      }
      case OpCode::LoadLocal:
        if (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= frame.locals.size()) return fail("R006", "local slot is outside the frame", instruction.span);
        stack_.push_back(frame.locals[static_cast<std::size_t>(instruction.operand)]);
        break;
      case OpCode::StoreLocal:
        if (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= frame.locals.size() || stack_.empty()) return fail("R006", "invalid local store", instruction.span);
        frame.locals[static_cast<std::size_t>(instruction.operand)] = stack_.back();
        break;
      case OpCode::LoadGlobal:
        if (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= globals.size()) return fail("R006", "global slot is outside the module", instruction.span);
        stack_.push_back(globals[static_cast<std::size_t>(instruction.operand)]);
        break;
      case OpCode::StoreGlobal:
        if (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= globals.size() || stack_.empty()) return fail("R006", "invalid global store", instruction.span);
        globals[static_cast<std::size_t>(instruction.operand)] = stack_.back();
        break;
      case OpCode::Call: {
        if (instruction.operand < 0 || static_cast<std::size_t>(instruction.operand) >= module.functions.size()) return fail("R006", "function index is outside the function table", instruction.span);
        const auto& function = module.functions[static_cast<std::size_t>(instruction.operand)];
        if (function.name == "print" || function.name == "input" || function.name == "len" ||
            function.name == "to_string" || function.name == "to_int") {
          std::vector<Value> args;
          const auto count = function.name == "input" ? 0U : (stack_.empty() ? 0U : 1U);
          for (unsigned index = 0; index < count; ++index) {
            const auto value = pop(instruction.span);
            if (!value.has_value()) return fail("R006", "builtin argument stack underflow", instruction.span);
            args.push_back(*value);
          }
          std::reverse(args.begin(), args.end());
          if (function.name == "print") {
            if (args.size() != 1) return fail("T004", "print expects one argument", instruction.span);
            output << value_to_string(args.front()) << '\n';
            stack_.emplace_back(UnitValue{});
          } else if (function.name == "input") {
            std::string line;
            std::getline(std::cin, line);
            stack_.emplace_back(std::move(line));
          } else if (function.name == "len") {
            if (args.size() != 1) return fail("R006", "len expects one argument", instruction.span);
            if (std::holds_alternative<std::string>(args.front())) {
              stack_.emplace_back(static_cast<std::int64_t>(std::get<std::string>(args.front()).size()));
            } else if (std::holds_alternative<std::shared_ptr<RuntimeArray>>(args.front()) &&
                       std::get<std::shared_ptr<RuntimeArray>>(args.front()) != nullptr) {
              stack_.emplace_back(static_cast<std::int64_t>(std::get<std::shared_ptr<RuntimeArray>>(args.front())->elements.size()));
            } else {
              return fail("R006", "len expects a string or array", instruction.span);
            }
          } else if (function.name == "to_string") {
            if (args.size() != 1) return fail("R006", "to_string expects one argument", instruction.span);
            stack_.emplace_back(value_to_string(args.front()));
          } else {
            if (args.size() != 1 || !std::holds_alternative<std::string>(args.front())) return fail("R005", "invalid integer input", instruction.span);
            const auto& text = std::get<std::string>(args.front());
            std::int64_t value = 0;
            const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
            if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) return fail("R005", "invalid integer input", instruction.span);
            stack_.emplace_back(value);
          }
          break;
        }
        if (frames.size() >= 1024) return fail("R004", "call stack overflow", instruction.span);
        if (stack_.size() < function.arity) return fail("R006", "call argument stack underflow", instruction.span);
        std::vector<Value> locals(function.local_count, UnitValue{});
        for (std::size_t index = 0; index < function.arity; ++index) {
          locals[function.arity - index - 1] = stack_.back();
          stack_.pop_back();
        }
        frames.push_back(Frame{static_cast<std::size_t>(instruction.operand), function.entry, frame.ip, std::move(locals)});
        break;
      }
      case OpCode::MakeArray: {
        if (instruction.operand < 0 || stack_.size() < static_cast<std::size_t>(instruction.operand)) {
          return fail("R006", "array construction stack underflow", instruction.span);
        }
        auto array = std::make_shared<RuntimeArray>();
        array->elements.resize(static_cast<std::size_t>(instruction.operand));
        for (std::size_t index = array->elements.size(); index > 0; --index) {
          array->elements[index - 1] = stack_.back();
          stack_.pop_back();
        }
        stack_.push_back(std::move(array));
        break;
      }
      case OpCode::Index: {
        const auto index = pop(instruction.span);
        const auto object = pop(instruction.span);
        if (!index.has_value() || !object.has_value() || !std::holds_alternative<std::int64_t>(*index) ||
            !std::holds_alternative<std::shared_ptr<RuntimeArray>>(*object)) {
          return fail("R006", "invalid index operation", instruction.span);
        }
        const auto array = std::get<std::shared_ptr<RuntimeArray>>(*object);
        const auto position = std::get<std::int64_t>(*index);
        if (array == nullptr || position < 0 || static_cast<std::size_t>(position) >= array->elements.size()) {
          return fail("R003", "index out of range", instruction.span);
        }
        stack_.push_back(array->elements[static_cast<std::size_t>(position)]);
        break;
      }
      case OpCode::Return: {
        const auto result = pop(instruction.span);
        if (!result.has_value()) return fail("R006", "return stack underflow", instruction.span);
        const auto return_ip = frame.return_ip;
        frames.pop_back();
        if (frames.empty()) return VmResult{true, *result, std::nullopt};
        frames.back().ip = return_ip;
        stack_.push_back(*result);
        break;
      }
      case OpCode::Pop:
        if (!pop(instruction.span).has_value()) return fail("R006", "pop stack underflow", instruction.span);
        break;
      case OpCode::Halt:
        return VmResult{true, stack_.empty() ? Value{UnitValue{}} : stack_.back(), std::nullopt};
    }
    if (stack_.size() > options_.max_stack) return fail("R004", "operand stack overflow", instruction.span);
  }
  return fail("R006", "VM stopped without HALT", Span::synthetic());
}

}  // namespace kite

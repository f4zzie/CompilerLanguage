#pragma once

#include "common/source.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace kite {

enum class IrOp { Constant, Add, Subtract, Multiply, Divide, Return };

struct IrInstruction {
  IrOp op{IrOp::Return};
  std::int64_t operand{0};
  Span span{Span::synthetic()};
};

struct IrFunction {
  std::string name;
  std::vector<IrInstruction> instructions;
};

}  // namespace kite

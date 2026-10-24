#pragma once

#include "codegen/bytecode.hpp"
#include "diagnostics/diagnostic.hpp"
#include "runtime/value.hpp"

#include <optional>
#include <ostream>
#include <vector>

namespace kite {

struct RuntimeError {
  std::string code;
  std::string message;
  Span span{Span::synthetic()};
};

struct VmOptions {
  bool trace{false};
  std::size_t max_stack{1024};
  std::ostream* trace_stream{nullptr};
  std::ostream* output_stream{nullptr};
};

struct VmResult {
  bool success{false};
  Value value{UnitValue{}};
  std::optional<RuntimeError> error;
};

class Vm {
 public:
  explicit Vm(VmOptions options = {}) : options_(options) {}
  VmResult run(const BytecodeModule& module);

 private:
  VmOptions options_;
  std::vector<Value> stack_;
};

}  // namespace kite

#include "doctest/doctest.h"

#include "runtime/vm.hpp"

#include <sstream>

TEST_CASE("vm adds integers and halts") {
  kite::BytecodeModule module;
  module.constants.emplace_back(std::int64_t{1});
  module.constants.emplace_back(std::int64_t{2});
  module.instructions = {
      {kite::OpCode::Constant, 0, kite::Span::synthetic()},
      {kite::OpCode::Constant, 1, kite::Span::synthetic()},
      {kite::OpCode::Add, 0, kite::Span::synthetic()},
      {kite::OpCode::Halt, 0, kite::Span::synthetic()},
  };
  module.functions.push_back(kite::FunctionInfo{"__main", 0, 0, 0});
  kite::Vm vm;
  const auto result = vm.run(module);
  REQUIRE(result.success);
  CHECK(std::get<std::int64_t>(result.value) == 3);
}

TEST_CASE("vm executes conditional jumps") {
  kite::BytecodeModule module;
  module.constants.emplace_back(true);
  module.constants.emplace_back(std::int64_t{7});
  module.instructions = {
      {kite::OpCode::Constant, 0, kite::Span::synthetic()},
      {kite::OpCode::JumpIfFalse, 4, kite::Span::synthetic()},
      {kite::OpCode::Constant, 1, kite::Span::synthetic()},
      {kite::OpCode::Halt, 0, kite::Span::synthetic()},
      {kite::OpCode::Unit, 0, kite::Span::synthetic()},
      {kite::OpCode::Halt, 0, kite::Span::synthetic()},
  };
  module.functions.push_back(kite::FunctionInfo{"__main", 0, 0, 0});
  const auto result = kite::Vm{}.run(module);
  REQUIRE(result.success);
  CHECK(std::get<std::int64_t>(result.value) == 7);
}

TEST_CASE("vm rejects malformed bytecode") {
  kite::BytecodeModule module;
  module.instructions.push_back({kite::OpCode::Constant, 1, kite::Span::synthetic()});
  module.functions.push_back(kite::FunctionInfo{"__main", 0, 0, 0});
  const auto result = kite::Vm{}.run(module);
  CHECK_FALSE(result.success);
  REQUIRE(result.error.has_value());
  CHECK(result.error->code == "R006");
}

TEST_CASE("vm trace is written to the trace stream") {
  kite::BytecodeModule module;
  module.instructions.push_back({kite::OpCode::Unit, 0, kite::Span::synthetic()});
  module.instructions.push_back({kite::OpCode::Halt, 0, kite::Span::synthetic()});
  module.functions.push_back(kite::FunctionInfo{"__main", 0, 0, 0});
  std::ostringstream trace;
  kite::Vm vm(kite::VmOptions{true, 1024, &trace});
  const auto result = vm.run(module);
  REQUIRE(result.success);
  CHECK(trace.str().find("UNIT") != std::string::npos);
}

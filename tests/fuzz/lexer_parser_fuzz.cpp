#include "driver/pipeline.hpp"
#include "runtime/vm.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

std::string nested_parens(std::size_t depth) {
  return "print(" + std::string(depth, '(') + "1" + std::string(depth, ')') + ");\n";
}

std::string nested_blocks(std::size_t depth) {
  return std::string(depth, '{') + std::string(depth, '}') + "\n";
}

std::string long_binary_chain(std::size_t terms) {
  std::string source = "print(";
  for (std::size_t index = 0; index < terms; ++index) {
    if (index != 0) source += "+";
    source += "1";
  }
  source += ");\n";
  return source;
}

bool has_code(const kite::Diagnostics& diagnostics, const std::string& code) {
  for (const auto& diagnostic : diagnostics.all()) {
    if (diagnostic.code == code) return true;
  }
  return false;
}

int run_case(const std::string& name, const std::string& source, bool expect_depth_error,
             bool expect_runtime_limit) {
  kite::Pipeline pipeline;
  const auto result = pipeline.compile(name, source, kite::PipelineOptions{false, 100, 200});
  if (expect_depth_error) {
    if (result.success || !has_code(result.diagnostics, "P004")) {
      std::cerr << name << " did not produce P004\n";
      return 1;
    }
    return 0;
  }
  if (!result.success) {
    std::cerr << name << " failed to compile\n";
    return 1;
  }

  std::ostringstream output;
  kite::VmOptions options;
  options.output_stream = &output;
  options.max_instructions = 10'000;
  const auto runtime = kite::Vm(options).run(result.module);
  if (expect_runtime_limit) {
    if (runtime.success || !runtime.error.has_value()) {
      std::cerr << name << " did not stop at a runtime limit\n";
      return 1;
    }
  } else if (!runtime.success) {
    std::cerr << name << " failed at runtime: " << runtime.error->message << '\n';
    return 1;
  }
  return 0;
}

}  // namespace

int main() {
  if (run_case("empty", "", false, false) != 0) return 1;
  if (run_case("binary", "1 + ;\n", false, false) == 0) {
    std::cerr << "binary input unexpectedly compiled\n";
    return 1;
  }

  const std::string huge_literal = "print(" + std::string(20'000, '9') + ");\n";
  if (run_case("huge-literal", huge_literal, false, false) == 0) {
    std::cerr << "huge literal unexpectedly compiled\n";
    return 1;
  }
  if (run_case("infinite-recursion", "fn loop() -> unit { loop(); } loop();\n", false, true) != 0) return 1;
  if (run_case("nested-parens", nested_parens(5'000), true, false) != 0) return 1;
  if (run_case("nested-blocks", nested_blocks(20'000), true, false) != 0) return 1;
  if (run_case("long-binary-chain", long_binary_chain(20'000), true, false) != 0) return 1;

  for (std::size_t iteration = 0; iteration < 128; ++iteration) {
    const auto depth = iteration % 180;
    std::string source = "print(" + std::string(depth, '(') + "1" + std::string(depth, ')') + ");\n";
    if (run_case("structured-" + std::to_string(iteration), source, false, false) != 0) return 1;
  }
  return 0;
}

#pragma once

#include "codegen/bytecode.hpp"
#include "diagnostics/diagnostic.hpp"

#include <string>

namespace kite {

struct PipelineOptions {
  bool trace{false};
  std::size_t max_errors{50};
};

struct PipelineResult {
  bool success{false};
  BytecodeModule module;
  Diagnostics diagnostics;
};

class Pipeline {
 public:
  PipelineResult compile(const std::string& file, const std::string& source,
                         PipelineOptions options = {}) const;
};

}  // namespace kite

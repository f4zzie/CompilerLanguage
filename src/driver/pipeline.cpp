#include "driver/pipeline.hpp"

#include "codegen/code_generator.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "sema/sema.hpp"

#include <utility>

namespace kite {

PipelineResult Pipeline::compile(const std::string& file, const std::string& source,
                                 PipelineOptions options) const {
  Diagnostics diagnostics(options.max_errors);
  Lexer lexer;
  Parser parser;
  SemanticAnalyzer sema;
  CodeGenerator generator;

  const auto tokens = lexer.scan(file, source, diagnostics);
  auto program = parser.parse(tokens, diagnostics);
  if (program != nullptr) {
    sema.analyze(*program, diagnostics);
  }
  BytecodeModule module;
  if (program != nullptr && !diagnostics.has_errors()) {
    module = generator.generate(*program, diagnostics);
  }
  diagnostics.sort_deterministically();
  return PipelineResult{!diagnostics.has_errors(), std::move(module), std::move(diagnostics)};
}

}  // namespace kite

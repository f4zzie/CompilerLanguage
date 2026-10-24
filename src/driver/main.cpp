#include "codegen/bytecode.hpp"
#include "codegen/code_generator.hpp"
#include "driver/cli.hpp"
#include "driver/pipeline.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token_dump.hpp"
#include "parser/ast_printer.hpp"
#include "parser/parser.hpp"
#include "runtime/vm.hpp"
#include "sema/sema.hpp"

#include <fstream>
#include <iostream>
#include <iterator>

namespace {

int usage_error(const std::string& message) {
  std::cerr << "kite: " << message << '\n';
  return 2;
}

struct Compilation {
  std::vector<kite::Token> tokens;
  std::unique_ptr<kite::Program> program;
  kite::BytecodeModule module;
  kite::Diagnostics diagnostics;
};

Compilation compile_file(const kite::CliOptions& options, const std::string& source) {
  kite::Diagnostics diagnostics(options.max_errors);
  kite::Lexer lexer;
  auto tokens = lexer.scan(options.file, source, diagnostics);
  kite::Parser parser;
  auto program = parser.parse(tokens, diagnostics);
  if (program != nullptr) {
    kite::SemanticAnalyzer analyzer;
    analyzer.analyze(*program, diagnostics);
  }
  kite::BytecodeModule module;
  if (program != nullptr && !diagnostics.has_errors()) {
    kite::CodeGenerator generator;
    module = generator.generate(*program, diagnostics);
  }
  diagnostics.sort_deterministically();
  return Compilation{std::move(tokens), std::move(program), std::move(module), std::move(diagnostics)};
}

}  // namespace

int main(int argc, char** argv) {
  const auto parsed = kite::parse_cli(argc, argv);
  if (!parsed.options.has_value()) {
    if (!parsed.error.empty()) {
      return usage_error(parsed.error);
    }
    kite::print_help();
    return parsed.error_code;
  }
  if (parsed.options->help) {
    kite::print_help();
    return 0;
  }

  const auto& options = *parsed.options;
  std::ifstream input(options.file, std::ios::binary);
  if (!input) {
    return usage_error("cannot open source file: " + options.file);
  }
  const std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  auto compilation = compile_file(options, source);
  for (const auto& diagnostic : compilation.diagnostics.all()) {
    if (diagnostic.severity == kite::DiagnosticSeverity::Note) {
      std::cerr << "note: " << diagnostic.message << '\n';
    } else {
      std::cerr << kite::render_diagnostic(diagnostic, source);
    }
  }
  if (compilation.diagnostics.has_errors()) {
    return 1;
  }

  if (options.command == "tokens") {
    std::cout << kite::dump_tokens(compilation.tokens);
    return 0;
  }
  if (options.command == "ast") {
    std::cout << kite::print_ast(*compilation.program);
    return 0;
  }
  if (options.command == "disasm") {
    std::cout << kite::disassemble(compilation.module);
    return 0;
  }
  if (options.command == "check") {
    return 0;
  }
  if (options.command == "build") {
    if (options.output_file.empty()) {
      return usage_error("build requires -o FILE");
    }
    std::ofstream output(options.output_file, std::ios::binary);
    if (!output) {
      return usage_error("cannot write output file: " + options.output_file);
    }
    output << kite::disassemble(compilation.module);
    return output.good() ? 0 : 4;
  }
  if (options.command == "run") {
    kite::VmOptions vm_options;
    vm_options.trace = options.trace;
    vm_options.trace_stream = &std::cerr;
    vm_options.output_stream = &std::cout;
    kite::Vm vm(vm_options);
    const auto result = vm.run(compilation.module);
    if (!result.success) {
      std::cerr << "runtime[" << result.error->code << "]: " << result.error->message << '\n';
      return 3;
    }
    return 0;
  }
  return usage_error("unknown command: " + options.command);
}

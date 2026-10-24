#include "codegen/bytecode.hpp"
#include "common/config.hpp"
#include "driver/pipeline.hpp"
#include "runtime/vm.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace {

void print_help() {
  std::cout << kite::kLanguageName << " compiler (scaffold)\n"
               "Usage: kite <command> <file.kite> [options]\n\n"
               "Commands: run, build, check, tokens, ast, disasm\n"
               "Options: --trace, --max-errors N, --color=auto|always|never\n";
}

int usage_error(const std::string& message) {
  std::cerr << "kite: " << message << '\n';
  return 2;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc == 1 || std::string(argv[1]) == "--help") {
    print_help();
    return 0;
  }
  if (argc < 3) {
    return usage_error("expected a command and a source file");
  }

  const std::string command = argv[1];
  const std::string file = argv[2];
  std::ifstream input(file);
  if (!input) {
    return usage_error("cannot open source file: " + file);
  }
  const std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  kite::Pipeline pipeline;
  const auto result = pipeline.compile(file, source);

  for (const auto& diagnostic : result.diagnostics.all()) {
    std::cerr << kite::render_diagnostic(diagnostic, source);
  }
  if (!result.success) {
    return 1;
  }

  if (command == "tokens") {
    std::cout << "<tokens unavailable in scaffold>\n";
    return 0;
  }
  if (command == "ast") {
    std::cout << "Program{}\n";
    return 0;
  }
  if (command == "disasm") {
    std::cout << kite::disassemble(result.module);
    return 0;
  }
  if (command == "check" || command == "build") {
    return 0;
  }
  if (command == "run") {
    kite::Vm vm;
    const auto vm_result = vm.run(result.module);
    if (!vm_result.success) {
      std::cerr << "runtime[" << vm_result.error->code << "]: " << vm_result.error->message << '\n';
      return 3;
    }
    return 0;
  }
  return usage_error("unknown command: " + command);
}

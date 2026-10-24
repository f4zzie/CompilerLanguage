#include "driver/cli.hpp"

#include <charconv>
#include <iostream>
#include <string_view>

namespace kite {

void print_help() {
  std::cout << "Kite compiler\n"
               "Usage: kite <command> <file.kite> [options]\n\n"
               "Commands: run, build, check, tokens, ast, disasm\n"
               "Options: --trace, --max-errors N, --color=auto|always|never, -o FILE\n";
}

CliParseResult parse_cli(int argc, char** argv) {
  if (argc == 1 || (argc == 2 && std::string_view(argv[1]) == "--help")) {
    return CliParseResult{CliOptions{"", "", "", false, 50, "auto", true}, {}, 0};
  }
  if (argc < 3) {
    return CliParseResult{std::nullopt, "expected a command and a source file", 2};
  }
  CliOptions options;
  options.command = argv[1];
  options.file = argv[2];
  const auto valid_command = options.command == "run" || options.command == "build" ||
                             options.command == "check" || options.command == "tokens" ||
                             options.command == "ast" || options.command == "disasm";
  if (!valid_command) {
    return CliParseResult{std::nullopt, "unknown command: " + options.command, 2};
  }
  for (int index = 3; index < argc; ++index) {
    const std::string_view argument(argv[index]);
    if (argument == "--trace") {
      options.trace = true;
    } else if (argument == "--max-errors") {
      if (index + 1 >= argc) return CliParseResult{std::nullopt, "--max-errors needs a number", 2};
      const std::string_view value(argv[++index]);
      std::size_t parsed = 0;
      const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
      if (result.ec != std::errc{} || result.ptr != value.data() + value.size() || parsed == 0) {
        return CliParseResult{std::nullopt, "--max-errors needs a positive number", 2};
      }
      options.max_errors = parsed;
    } else if (argument.starts_with("--color=")) {
      options.color = std::string(argument.substr(8));
      if (options.color != "auto" && options.color != "always" && options.color != "never") {
        return CliParseResult{std::nullopt, "--color must be auto, always, or never", 2};
      }
    } else if (argument == "-o" || argument == "--output") {
      if (index + 1 >= argc) return CliParseResult{std::nullopt, "-o needs a file", 2};
      options.output_file = argv[++index];
    } else {
      return CliParseResult{std::nullopt, "unknown option: " + std::string(argument), 2};
    }
  }
  return CliParseResult{options, {}, 0};
}

}  // namespace kite

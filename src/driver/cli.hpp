#pragma once

#include <optional>
#include <string>

namespace kite {

struct CliOptions {
  std::string command;
  std::string file;
  std::string output_file;
  bool trace{false};
  std::size_t max_errors{50};
  std::string color{"auto"};
  bool help{false};
};

struct CliParseResult {
  std::optional<CliOptions> options;
  std::string error;
  int error_code{0};
};

CliParseResult parse_cli(int argc, char** argv);
void print_help();

}  // namespace kite

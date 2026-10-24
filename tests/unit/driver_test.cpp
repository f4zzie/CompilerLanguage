#include "doctest/doctest.h"

#include "driver/cli.hpp"

TEST_CASE("cli parses command, file, and trace options") {
  const char* arguments[] = {"kite", "run", "examples/hello.kite", "--trace", "--max-errors", "7"};
  const auto result = kite::parse_cli(6, const_cast<char**>(arguments));
  REQUIRE(result.options.has_value());
  CHECK(result.options->command == "run");
  CHECK(result.options->file == "examples/hello.kite");
  CHECK(result.options->trace);
  CHECK(result.options->max_errors == 7);
}

TEST_CASE("cli treats no arguments as help") {
  const char* arguments[] = {"kite"};
  const auto result = kite::parse_cli(1, const_cast<char**>(arguments));
  REQUIRE(result.options.has_value());
  CHECK(result.options->help);
  CHECK(result.error_code == 0);
}

TEST_CASE("cli accepts stage dump commands") {
  const char* arguments[] = {"kite", "tokens", "file.kite"};
  const auto result = kite::parse_cli(3, const_cast<char**>(arguments));
  REQUIRE(result.options.has_value());
  CHECK(result.options->command == "tokens");
}

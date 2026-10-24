#include "doctest/doctest.h"

#include "lexer/lexer.hpp"

TEST_CASE("empty source produces eof") {
  kite::Diagnostics diagnostics;
  kite::Lexer lexer;
  const auto tokens = lexer.scan("<test>", "", diagnostics);
  CHECK(tokens.size() == 1);
  CHECK(tokens.front().kind == kite::TokenKind::Eof);
}

TEST_CASE("walking_skeleton") {
  // TODO(T014): replace this red test with an end-to-end print(1 + 2) assertion.
  CHECK(false);
}

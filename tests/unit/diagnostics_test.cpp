#include "doctest/doctest.h"

#include "diagnostics/diagnostic.hpp"

TEST_CASE("diagnostics collect errors") {
  kite::Diagnostics diagnostics;
  diagnostics.error("T001", "type mismatch", kite::Span::synthetic());
  CHECK(diagnostics.has_errors());
  CHECK(diagnostics.error_count() == 1);
}

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

TEST_CASE("test binary starts and registers tests") {
  // Executing this registered case proves the test binary started and doctest registered it.
  CHECK(true);
}

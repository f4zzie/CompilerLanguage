#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

TEST_CASE("test binary starts and registers tests") {
  CHECK(!doctest::registry().empty());
}

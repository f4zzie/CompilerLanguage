#pragma once

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace doctest {

struct TestCase {
  const char* name;
  void (*function)();
};

inline std::vector<TestCase>& registry() {
  static std::vector<TestCase> tests;
  return tests;
}

struct Registrar {
  Registrar(const char* name, void (*function)()) { registry().push_back(TestCase{name, function}); }
};

inline int run_tests(int argc, char** argv) {
  std::string filter;
  std::string exclude;
  for (int index = 1; index < argc; ++index) {
    const std::string_view argument(argv[index]);
    constexpr std::string_view prefix = "--test-case=";
    if (argument.starts_with(prefix)) {
      filter = std::string(argument.substr(prefix.size()));
    }
    constexpr std::string_view exclude_prefix = "--test-case-exclude=";
    if (argument.starts_with(exclude_prefix)) {
      exclude = std::string(argument.substr(exclude_prefix.size()));
    }
  }
  int failures = 0;
  for (const auto& test : registry()) {
    if (!filter.empty() && filter != test.name) {
      continue;
    }
    if (!exclude.empty() && exclude == test.name) {
      continue;
    }
    try {
      test.function();
    } catch (const std::exception& exception) {
      ++failures;
      std::cerr << "[doctest] " << test.name << ": " << exception.what() << '\n';
    } catch (...) {
      ++failures;
      std::cerr << "[doctest] " << test.name << ": unknown exception\n";
    }
  }
  return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

}  // namespace doctest

#define DOCTEST_DETAIL_JOIN_IMPL(left, right) left##right
#define DOCTEST_DETAIL_JOIN(left, right) DOCTEST_DETAIL_JOIN_IMPL(left, right)
#define TEST_CASE(name)                                                                            \
  static void DOCTEST_DETAIL_JOIN(doctest_case_, __LINE__)();                                     \
  static ::doctest::Registrar DOCTEST_DETAIL_JOIN(doctest_registrar_, __LINE__)(                 \
      name, &DOCTEST_DETAIL_JOIN(doctest_case_, __LINE__));                                      \
  static void DOCTEST_DETAIL_JOIN(doctest_case_, __LINE__)()
#define CHECK(expression)                                                                          \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      std::cerr << "CHECK failed: " #expression " at " << __FILE__ << ':' << __LINE__ << '\n';   \
      throw std::runtime_error("check failed");                                                   \
    }                                                                                              \
  } while (false)
#define REQUIRE(expression) CHECK(expression)

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
int main(int argc, char** argv) { return ::doctest::run_tests(argc, argv); }
#endif

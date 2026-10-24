#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

#include <cstdint>
#include <iostream>
#include <random>
#include <string>

int main() {
  std::mt19937 generator(0x4b495445U);
  std::uniform_int_distribution<int> length_distribution(0, 256);
  std::uniform_int_distribution<int> byte_distribution(0, 255);
  for (int iteration = 0; iteration < 1000; ++iteration) {
    std::string source;
    source.resize(static_cast<std::size_t>(length_distribution(generator)));
    for (char& byte : source) {
      byte = static_cast<char>(byte_distribution(generator));
    }
    kite::Diagnostics diagnostics(100);
    kite::Lexer lexer;
    const auto tokens = lexer.scan("<fuzz>", source, diagnostics);
    kite::Parser parser;
    auto program = parser.parse(tokens, diagnostics);
    if (program == nullptr) {
      std::cerr << "parser returned null at iteration " << iteration << '\n';
      return 1;
    }
  }
  return 0;
}

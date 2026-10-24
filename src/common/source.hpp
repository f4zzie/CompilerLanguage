#pragma once

#include <cstddef>
#include <string>

namespace kite {

struct SourceLocation {
  std::string file;
  std::size_t line{1};
  std::size_t column{1};
  std::size_t offset{0};
};

struct Span {
  SourceLocation start;
  SourceLocation end;

  static Span synthetic() {
    return Span{SourceLocation{"<synthetic>", 1, 1, 0},
                SourceLocation{"<synthetic>", 1, 1, 0}};
  }
};

}  // namespace kite

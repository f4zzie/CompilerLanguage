#pragma once

#include "common/source.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace kite {

class SourceManager {
 public:
  void add(std::string file, std::string source);
  [[nodiscard]] std::string_view source_for(std::string_view file) const;
  [[nodiscard]] std::string line_for(const Span& span) const;

 private:
  std::unordered_map<std::string, std::string> sources_;
};

}  // namespace kite

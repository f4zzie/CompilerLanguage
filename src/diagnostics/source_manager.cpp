#include "diagnostics/source_manager.hpp"

#include <utility>

namespace kite {

void SourceManager::add(std::string file, std::string source) {
  sources_[std::move(file)] = std::move(source);
}

std::string_view SourceManager::source_for(std::string_view file) const {
  const auto iterator = sources_.find(std::string(file));
  return iterator == sources_.end() ? std::string_view{} : std::string_view(iterator->second);
}

std::string SourceManager::line_for(const Span& span) const {
  const auto source = source_for(span.start.file);
  const auto begin = source.rfind('\n', span.start.offset);
  const auto line_begin = begin == std::string_view::npos ? 0 : begin + 1;
  const auto end = source.find('\n', line_begin);
  return std::string(source.substr(line_begin, end == std::string_view::npos
                                             ? source.size() - line_begin
                                             : end - line_begin));
}

}  // namespace kite

#include "diagnostics/diagnostic.hpp"

#include <algorithm>
#include <sstream>
#include <string_view>
#include <utility>

namespace kite {

void Diagnostics::add(Diagnostic diagnostic) {
  if (diagnostic.severity == DiagnosticSeverity::Error && error_count() >= max_errors_) {
    if (!limit_reported_) {
      diagnostics_.push_back(Diagnostic{
          DiagnosticSeverity::Note,
          "E999",
          "maximum error limit reached; further errors suppressed",
          Span::synthetic(),
          {},
          {},
      });
      limit_reported_ = true;
    }
    return;
  }
  diagnostics_.push_back(std::move(diagnostic));
}

void Diagnostics::error(std::string code, std::string message, Span span) {
  add(Diagnostic{DiagnosticSeverity::Error, std::move(code), std::move(message), std::move(span), {}, {}});
}

bool Diagnostics::has_errors() const {
  return std::any_of(diagnostics_.begin(), diagnostics_.end(), [](const Diagnostic& diagnostic) {
    return diagnostic.severity == DiagnosticSeverity::Error;
  });
}

std::size_t Diagnostics::error_count() const {
  return static_cast<std::size_t>(std::count_if(
      diagnostics_.begin(), diagnostics_.end(), [](const Diagnostic& diagnostic) {
        return diagnostic.severity == DiagnosticSeverity::Error;
      }));
}

const std::vector<Diagnostic>& Diagnostics::all() const {
  return diagnostics_;
}

void Diagnostics::sort_deterministically() {
  std::stable_sort(diagnostics_.begin(), diagnostics_.end(), [](const Diagnostic& left,
                                                                const Diagnostic& right) {
    if (left.primary_span.start.file != right.primary_span.start.file) {
      return left.primary_span.start.file < right.primary_span.start.file;
    }
    if (left.primary_span.start.line != right.primary_span.start.line) {
      return left.primary_span.start.line < right.primary_span.start.line;
    }
    if (left.primary_span.start.column != right.primary_span.start.column) {
      return left.primary_span.start.column < right.primary_span.start.column;
    }
    return left.code < right.code;
  });
}

std::string render_diagnostic(const Diagnostic& diagnostic, std::string_view source) {
  std::ostringstream output;
  const auto line_start = source.rfind('\n', diagnostic.primary_span.start.offset);
  const auto line_begin = line_start == std::string_view::npos ? 0 : line_start + 1;
  const auto line_end = source.find('\n', line_begin);
  const auto line = source.substr(line_begin, line_end == std::string_view::npos
                                                   ? source.size() - line_begin
                                                   : line_end - line_begin);
  output << diagnostic.primary_span.start.file << ':' << diagnostic.primary_span.start.line << ':'
         << diagnostic.primary_span.start.column << ": " << diagnostic.code << ' '
         << diagnostic.message << '\n';
  output << line << '\n';
  const auto caret_column = diagnostic.primary_span.start.column > 0
                                ? diagnostic.primary_span.start.column - 1
                                : 0;
  output << std::string(caret_column, ' ') << '^';
  const auto width = diagnostic.primary_span.end.offset > diagnostic.primary_span.start.offset
                         ? diagnostic.primary_span.end.offset - diagnostic.primary_span.start.offset
                         : 1;
  if (width > 1) {
    output << std::string(width - 1, '~');
  }
  output << '\n';
  return output.str();
}

}  // namespace kite

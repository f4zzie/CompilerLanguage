#pragma once

#include "common/source.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace kite {

enum class DiagnosticSeverity { Note, Warning, Error };

struct DiagnosticNote {
  std::string message;
  Span span{Span::synthetic()};
};

struct Diagnostic {
  DiagnosticSeverity severity{DiagnosticSeverity::Error};
  std::string code;
  std::string message;
  Span primary_span{Span::synthetic()};
  std::vector<DiagnosticNote> notes;
  std::string hint;
};

class Diagnostics {
 public:
  explicit Diagnostics(std::size_t max_errors = 50) : max_errors_(max_errors) {}

  void add(Diagnostic diagnostic);
  void error(std::string code, std::string message, Span span);
  [[nodiscard]] bool has_errors() const;
  [[nodiscard]] std::size_t error_count() const;
  [[nodiscard]] const std::vector<Diagnostic>& all() const;
  void sort_deterministically();

 private:
  std::size_t max_errors_;
  bool limit_reported_{false};
  std::vector<Diagnostic> diagnostics_;
};

std::string render_diagnostic(const Diagnostic& diagnostic, std::string_view source);

}  // namespace kite

#include "doctest/doctest.h"

#include "diagnostics/diagnostic.hpp"
#include "diagnostics/source_manager.hpp"

TEST_CASE("diagnostics collect errors") {
  kite::Diagnostics diagnostics;
  diagnostics.error("T001", "type mismatch", kite::Span::synthetic());
  CHECK(diagnostics.has_errors());
  CHECK(diagnostics.error_count() == 1);
}

TEST_CASE("diagnostic renders a single-line caret") {
  const std::string source = "let value = 1;\n";
  const kite::Span span{
      kite::SourceLocation{"sample.kite", 1, 5, 4},
      kite::SourceLocation{"sample.kite", 1, 10, 9},
  };
  const kite::Diagnostic diagnostic{
      kite::DiagnosticSeverity::Error,
      "T001",
      "type mismatch",
      span,
      {},
      {},
  };
  CHECK(kite::render_diagnostic(diagnostic, source) ==
        "sample.kite:1:5: T001 type mismatch\nlet value = 1;\n    ^~~~~\n");
}

TEST_CASE("diagnostic renders the correct line from a multi-line source") {
  const std::string source = "first;\nsecond;\nthird;\n";
  const kite::Span span{
      kite::SourceLocation{"sample.kite", 2, 1, 7},
      kite::SourceLocation{"sample.kite", 2, 7, 13},
  };
  const kite::Diagnostic diagnostic{
      kite::DiagnosticSeverity::Error,
      "P002",
      "expected semicolon",
      span,
      {},
      {},
  };
  CHECK(kite::render_diagnostic(diagnostic, source) ==
        "sample.kite:2:1: P002 expected semicolon\nsecond;\n^~~~~~\n");
}

TEST_CASE("diagnostics cap errors and report suppression") {
  kite::Diagnostics diagnostics(1);
  diagnostics.error("L001", "bad character", kite::Span::synthetic());
  diagnostics.error("L001", "another bad character", kite::Span::synthetic());
  CHECK(diagnostics.error_count() == 1);
  CHECK(diagnostics.all().size() == 2);
  CHECK(diagnostics.all().back().severity == kite::DiagnosticSeverity::Note);
  CHECK(diagnostics.all().back().message.find("maximum error limit") != std::string::npos);
}

TEST_CASE("source manager returns stored source and line") {
  kite::SourceManager sources;
  sources.add("sample.kite", "one\ntwo\n");
  const kite::Span span{
      kite::SourceLocation{"sample.kite", 2, 1, 4},
      kite::SourceLocation{"sample.kite", 2, 4, 7},
  };
  CHECK(sources.source_for("sample.kite") == "one\ntwo\n");
  CHECK(sources.line_for(span) == "two");
}

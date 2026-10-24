#include "doctest/doctest.h"

#include "sema/symbol.hpp"

TEST_CASE("symbol table supports nested shadowing") {
  kite::SymbolTable table;
  table.push_scope();
  const kite::Symbol outer{"value", kite::Type{kite::TypeKind::Int, nullptr}, false,
                           kite::Span::synthetic()};
  CHECK(table.insert(outer));
  CHECK(table.lookup("value") == &*table.lookup("value"));
  table.push_scope();
  const kite::Symbol inner{"value", kite::Type{kite::TypeKind::Bool, nullptr}, false,
                           kite::Span::synthetic()};
  CHECK(table.insert(inner));
  CHECK(table.lookup("value")->type.kind == kite::TypeKind::Bool);
  table.pop_scope();
  CHECK(table.lookup("value")->type.kind == kite::TypeKind::Int);
}

TEST_CASE("symbol table rejects duplicate declarations in one scope") {
  kite::SymbolTable table;
  const kite::Symbol symbol{"value", kite::Type{kite::TypeKind::Int, nullptr}, false,
                            kite::Span::synthetic()};
  CHECK(table.insert(symbol));
  CHECK_FALSE(table.insert(symbol));
}

TEST_CASE("symbol table returns null for unresolved names") {
  kite::SymbolTable table;
  table.push_scope();
  CHECK(table.lookup("missing") == nullptr);
}

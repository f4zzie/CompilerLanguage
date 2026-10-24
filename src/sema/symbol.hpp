#pragma once

#include "parser/ast.hpp"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace kite {

struct Symbol {
  std::string name;
  Type type{Type::error()};
  bool is_function{false};
  Span declared_at{Span::synthetic()};
};

class Scope {
 public:
  explicit Scope(Scope* parent = nullptr) : parent_(parent) {}
  bool insert(Symbol symbol);
  const Symbol* lookup(const std::string& name) const;

 private:
  Scope* parent_;
  std::unordered_map<std::string, Symbol> symbols_;
};

class SymbolTable {
 public:
  void push_scope();
  void pop_scope();
  bool insert(Symbol symbol);
  const Symbol* lookup(const std::string& name) const;

 private:
  std::vector<std::unique_ptr<Scope>> scopes_;
};

}  // namespace kite

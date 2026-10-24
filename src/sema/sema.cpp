#include "sema/sema.hpp"

#include <utility>

namespace kite {

bool Scope::insert(Symbol symbol) {
  return symbols_.emplace(symbol.name, std::move(symbol)).second;
}

const Symbol* Scope::lookup(const std::string& name) const {
  const auto local = symbols_.find(name);
  if (local != symbols_.end()) {
    return &local->second;
  }
  return parent_ == nullptr ? nullptr : parent_->lookup(name);
}

void SymbolTable::push_scope() {
  scopes_.push_back(std::make_unique<Scope>(scopes_.empty() ? nullptr : scopes_.back().get()));
}

void SymbolTable::pop_scope() {
  if (!scopes_.empty()) {
    scopes_.pop_back();
  }
}

bool SymbolTable::insert(Symbol symbol) {
  if (scopes_.empty()) {
    push_scope();
  }
  return scopes_.back()->insert(std::move(symbol));
}

const Symbol* SymbolTable::lookup(const std::string& name) const {
  return scopes_.empty() ? nullptr : scopes_.back()->lookup(name);
}

SemanticResult SemanticAnalyzer::analyze(Program& /*program*/, Diagnostics& /*diagnostics*/) {
  // TODO(T008): collect declarations, resolve names, and annotate expression types.
  return SemanticResult{true};
}

}  // namespace kite

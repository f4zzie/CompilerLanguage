#pragma once

#include "diagnostics/diagnostic.hpp"
#include "parser/ast.hpp"
#include "sema/symbol.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace kite {

struct SemanticResult {
  bool success{false};
};

struct FunctionSignature {
  std::string name;
  std::vector<Type> parameters;
  Type return_type{Type::error()};
};

class SemanticAnalyzer {
 public:
  SemanticResult analyze(Program& program, Diagnostics& diagnostics);

 private:
  Type check_expression(Expr& expression, SymbolTable& symbols, Diagnostics& diagnostics);
  bool check_statement(Stmt& statement, SymbolTable& symbols, const Type& return_type,
                       Diagnostics& diagnostics);
  Type type_from_annotation(const TypeAnnotation& annotation) const;
  void report(Diagnostics& diagnostics, std::string code, std::string message, Span span) const;

  std::unordered_map<std::string, FunctionSignature> functions_;
};

}  // namespace kite

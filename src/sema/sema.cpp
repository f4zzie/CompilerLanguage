#include "sema/sema.hpp"

#include "sema/type.hpp"

#include <utility>

namespace kite {

namespace {

bool is_numeric(const Type& type) {
  return type.kind == TypeKind::Int || type.kind == TypeKind::Float;
}

bool is_error(const Type& type) { return type.kind == TypeKind::Error; }

bool is_assignable_expression(const Expr& expression) {
  return dynamic_cast<const NameExpr*>(&expression) != nullptr ||
         dynamic_cast<const IndexExpr*>(&expression) != nullptr;
}

bool contains_value_return(const Stmt& statement) {
  if (const auto* result = dynamic_cast<const ReturnStmt*>(&statement)) {
    return result->value != nullptr;
  }
  if (const auto* block = dynamic_cast<const BlockStmt*>(&statement)) {
    for (const auto& child : block->statements) {
      if (contains_value_return(*child)) {
        return true;
      }
    }
  }
  if (const auto* conditional = dynamic_cast<const IfStmt*>(&statement)) {
    return contains_value_return(*conditional->then_branch) ||
           (conditional->else_branch != nullptr && contains_value_return(*conditional->else_branch));
  }
  return false;
}

}  // namespace

SemanticResult SemanticAnalyzer::analyze(Program& program, Diagnostics& diagnostics) {
  functions_.clear();
  for (const auto& declaration : program.declarations) {
    const auto* function = dynamic_cast<const FunctionDecl*>(declaration.get());
    if (function == nullptr) {
      continue;
    }
    if (functions_.contains(function->name)) {
      report(diagnostics, "S001", "duplicate declaration '" + function->name + "'", function->span);
      continue;
    }
    FunctionSignature signature;
    signature.name = function->name;
    for (const auto& parameter : function->parameters) {
      signature.parameters.push_back(type_from_annotation(parameter.annotation));
    }
    signature.return_type = type_from_annotation(function->return_type);
    functions_.emplace(function->name, std::move(signature));
  }

  SymbolTable globals;
  globals.push_scope();
  for (const auto& [name, signature] : functions_) {
    globals.insert(Symbol{name, signature.return_type, true, Span::synthetic()});
  }

  for (const auto& declaration : program.declarations) {
    if (const auto* function = dynamic_cast<const FunctionDecl*>(declaration.get())) {
      SymbolTable symbols;
      symbols.push_scope();
      for (const auto& [name, signature] : functions_) {
        symbols.insert(Symbol{name, signature.return_type, true, Span::synthetic()});
      }
      symbols.push_scope();
      const auto signature_iterator = functions_.find(function->name);
      const auto return_type = signature_iterator == functions_.end()
                                   ? Type::error()
                                   : signature_iterator->second.return_type;
      for (std::size_t index = 0; index < function->parameters.size(); ++index) {
        const auto& parameter = function->parameters[index];
        symbols.insert(Symbol{parameter.name, type_from_annotation(parameter.annotation), false,
                              parameter.span});
      }
      const bool returned = check_statement(*function->body, symbols, return_type, diagnostics);
      if (return_type.kind != TypeKind::Unit && !returned) {
        report(diagnostics, "T008", "missing return", function->span);
      }
      if (contains_value_return(*function->body) && return_type.kind == TypeKind::Unit) {
        report(diagnostics, "T006", "function may not return a value", function->span);
      }
    }
  }

  for (const auto& statement : program.statements) {
    check_statement(*statement, globals, Type::unit(), diagnostics);
  }
  diagnostics.sort_deterministically();
  return SemanticResult{!diagnostics.has_errors()};
}

Type SemanticAnalyzer::check_expression(Expr& expression, SymbolTable& symbols,
                                         Diagnostics& diagnostics) {
  if (auto* literal = dynamic_cast<LiteralExpr*>(&expression)) {
    switch (literal->kind) {
      case LiteralKind::Integer:
        literal->inferred_type = Type{TypeKind::Int, nullptr};
        break;
      case LiteralKind::Float:
        literal->inferred_type = Type{TypeKind::Float, nullptr};
        break;
      case LiteralKind::String:
        literal->inferred_type = Type{TypeKind::String, nullptr};
        break;
      case LiteralKind::Boolean:
        literal->inferred_type = Type{TypeKind::Bool, nullptr};
        break;
      case LiteralKind::Unit:
        literal->inferred_type = Type::unit();
        break;
    }
    return *literal->inferred_type;
  }
  if (auto* name = dynamic_cast<NameExpr*>(&expression)) {
    if (const auto* symbol = symbols.lookup(name->name)) {
      name->inferred_type = symbol->type;
      return symbol->type;
    }
    const auto function = functions_.find(name->name);
    if (function != functions_.end()) {
      name->inferred_type = function->second.return_type;
      return function->second.return_type;
    }
    report(diagnostics, "S002", "unresolved name '" + name->name + "'", name->span);
    name->inferred_type = Type::error();
    return Type::error();
  }
  if (auto* unary = dynamic_cast<UnaryExpr*>(&expression)) {
    const auto operand = check_expression(*unary->operand, symbols, diagnostics);
    const bool valid = unary->op == UnaryOperator::Not ? operand.kind == TypeKind::Bool
                                                        : is_numeric(operand);
    if (!valid && !is_error(operand)) {
      report(diagnostics, "T007", "operator does not accept operand type " + type_name(operand),
             unary->span);
      unary->inferred_type = Type::error();
      return Type::error();
    }
    unary->inferred_type = operand;
    return operand;
  }
  if (auto* binary = dynamic_cast<BinaryExpr*>(&expression)) {
    const auto left = check_expression(*binary->left, symbols, diagnostics);
    const auto right = check_expression(*binary->right, symbols, diagnostics);
    Type result = Type::error();
    const bool matching = types_equal(left, right);
    switch (binary->op) {
      case BinaryOperator::Add:
        if ((matching && is_numeric(left)) || (left.kind == TypeKind::String && right.kind == TypeKind::String)) {
          result = left;
        }
        break;
      case BinaryOperator::Subtract:
      case BinaryOperator::Multiply:
      case BinaryOperator::Divide:
      case BinaryOperator::Modulo:
        if (matching && is_numeric(left)) {
          result = left;
        }
        break;
      case BinaryOperator::Equal:
      case BinaryOperator::NotEqual:
        if (matching) {
          result = Type{TypeKind::Bool, nullptr};
        }
        break;
      case BinaryOperator::Less:
      case BinaryOperator::LessEqual:
      case BinaryOperator::Greater:
      case BinaryOperator::GreaterEqual:
        if (matching && is_numeric(left)) {
          result = Type{TypeKind::Bool, nullptr};
        }
        break;
      case BinaryOperator::And:
      case BinaryOperator::Or:
        if (left.kind == TypeKind::Bool && right.kind == TypeKind::Bool) {
          result = Type{TypeKind::Bool, nullptr};
        }
        break;
    }
    if (result.kind == TypeKind::Error && !is_error(left) && !is_error(right)) {
      report(diagnostics, "T007", "operator does not accept " + type_name(left) + " and " + type_name(right),
             binary->span);
    }
    binary->inferred_type = result;
    return result;
  }
  if (auto* assignment = dynamic_cast<AssignmentExpr*>(&expression)) {
    const auto target = check_expression(*assignment->target, symbols, diagnostics);
    const auto value = check_expression(*assignment->value, symbols, diagnostics);
    if (!is_assignable_expression(*assignment->target)) {
      report(diagnostics, "P003", "invalid assignment target", assignment->target->span);
    } else if (!is_error(target) && !is_error(value) && !types_equal(target, value)) {
      report(diagnostics, "T001", "type mismatch: expected " + type_name(target) + ", found " + type_name(value),
             assignment->span);
    }
    assignment->inferred_type = Type::unit();
    return Type::unit();
  }
  if (auto* call = dynamic_cast<CallExpr*>(&expression)) {
    const auto function = functions_.find(call->callee);
    if (function == functions_.end()) {
      if (call->callee == "print" || call->callee == "input" || call->callee == "len" ||
          call->callee == "to_string" || call->callee == "to_int") {
        Type result = Type::unit();
        if (call->callee == "input") {
          result = Type{TypeKind::String, nullptr};
        } else if (call->callee == "len" || call->callee == "to_int") {
          result = Type{TypeKind::Int, nullptr};
        } else if (call->callee == "to_string") {
          result = Type{TypeKind::String, nullptr};
        }
        for (auto& argument : call->arguments) {
          check_expression(*argument, symbols, diagnostics);
        }
        call->inferred_type = result;
        return result;
      }
      report(diagnostics, "S002", "unresolved name '" + call->callee + "'", call->span);
      call->inferred_type = Type::error();
      return Type::error();
    }
    if (call->arguments.size() != function->second.parameters.size()) {
      report(diagnostics, "T004", "wrong argument count: expected " +
                                      std::to_string(function->second.parameters.size()) + ", found " +
                                      std::to_string(call->arguments.size()), call->span);
    }
    const auto count = std::min(call->arguments.size(), function->second.parameters.size());
    for (std::size_t index = 0; index < call->arguments.size(); ++index) {
      const auto actual = check_expression(*call->arguments[index], symbols, diagnostics);
      if (index < count && !is_error(actual) && !types_equal(actual, function->second.parameters[index])) {
        report(diagnostics, "T005", "argument " + std::to_string(index + 1) + " has type " + type_name(actual) +
                                        ", expected " + type_name(function->second.parameters[index]),
               call->arguments[index]->span);
      }
    }
    call->inferred_type = function->second.return_type;
    return function->second.return_type;
  }
  if (auto* index = dynamic_cast<IndexExpr*>(&expression)) {
    const auto object = check_expression(*index->object, symbols, diagnostics);
    const auto subscript = check_expression(*index->index, symbols, diagnostics);
    if (subscript.kind != TypeKind::Int && !is_error(subscript)) {
      report(diagnostics, "T007", "index must have type int", index->index->span);
    }
    if (object.kind == TypeKind::String) {
      index->inferred_type = Type{TypeKind::String, nullptr};
      return *index->inferred_type;
    }
    if (object.kind == TypeKind::Array && object.element != nullptr) {
      index->inferred_type = *object.element;
      return *index->inferred_type;
    }
    index->inferred_type = Type::error();
    return Type::error();
  }
  if (auto* array = dynamic_cast<ArrayExpr*>(&expression)) {
    Type element = Type::error();
    for (auto& item : array->elements) {
      const auto item_type = check_expression(*item, symbols, diagnostics);
      if (element.kind == TypeKind::Error) {
        element = item_type;
      } else if (!types_equal(element, item_type) && !is_error(item_type)) {
        report(diagnostics, "T001", "array elements must have one type", item->span);
      }
    }
    array->inferred_type = Type{TypeKind::Array, std::make_shared<Type>(element)};
    return *array->inferred_type;
  }
  if (auto* pipe = dynamic_cast<PipeExpr*>(&expression)) {
    const auto value = check_expression(*pipe->value, symbols, diagnostics);
    const auto function = functions_.find(pipe->function_name);
    if (function == functions_.end()) {
      report(diagnostics, "S002", "unresolved name '" + pipe->function_name + "'", pipe->span);
      pipe->inferred_type = Type::error();
      return Type::error();
    }
    if (function->second.parameters.empty() || !types_equal(value, function->second.parameters.front())) {
      report(diagnostics, "T005", "pipe value has incompatible type", pipe->span);
    }
    pipe->inferred_type = function->second.return_type;
    return function->second.return_type;
  }
  return Type::error();
}

bool SemanticAnalyzer::check_statement(Stmt& statement, SymbolTable& symbols, const Type& return_type,
                                       Diagnostics& diagnostics) {
  if (auto* expression = dynamic_cast<ExprStmt*>(&statement)) {
    check_expression(*expression->expression, symbols, diagnostics);
    return false;
  }
  if (auto* let = dynamic_cast<LetStmt*>(&statement)) {
    const auto inferred = check_expression(*let->initializer, symbols, diagnostics);
    let->inferred_type = let->annotation.has_value() ? type_from_annotation(*let->annotation) : inferred;
    if (let->annotation.has_value() && !is_error(inferred) && !types_equal(let->inferred_type, inferred)) {
      report(diagnostics, "T001", "type mismatch: expected " + type_name(let->inferred_type) + ", found " +
                                      type_name(inferred), let->initializer->span);
    }
    if (!symbols.insert(Symbol{let->name, let->inferred_type, false, let->span})) {
      report(diagnostics, "S001", "duplicate declaration '" + let->name + "'", let->span);
    }
    return false;
  }
  if (auto* assignment = dynamic_cast<AssignStmt*>(&statement)) {
    check_expression(*assignment->target, symbols, diagnostics);
    check_expression(*assignment->value, symbols, diagnostics);
    return false;
  }
  if (auto* block = dynamic_cast<BlockStmt*>(&statement)) {
    symbols.push_scope();
    bool returned = false;
    for (auto& child : block->statements) {
      returned = check_statement(*child, symbols, return_type, diagnostics) || returned;
    }
    symbols.pop_scope();
    return returned;
  }
  if (auto* conditional = dynamic_cast<IfStmt*>(&statement)) {
    const auto condition = check_expression(*conditional->condition, symbols, diagnostics);
    if (condition.kind != TypeKind::Bool && !is_error(condition)) {
      report(diagnostics, "T002", "condition must have type bool", conditional->condition->span);
    }
    const bool then_returns = check_statement(*conditional->then_branch, symbols, return_type, diagnostics);
    const bool else_returns = conditional->else_branch != nullptr &&
                              check_statement(*conditional->else_branch, symbols, return_type, diagnostics);
    return then_returns && else_returns;
  }
  if (auto* loop = dynamic_cast<WhileStmt*>(&statement)) {
    const auto condition = check_expression(*loop->condition, symbols, diagnostics);
    if (condition.kind != TypeKind::Bool && !is_error(condition)) {
      report(diagnostics, "T002", "condition must have type bool", loop->condition->span);
    }
    check_statement(*loop->body, symbols, return_type, diagnostics);
    return false;
  }
  if (auto* loop = dynamic_cast<ForStmt*>(&statement)) {
    symbols.push_scope();
    if (loop->initializer != nullptr) {
      check_statement(*loop->initializer, symbols, return_type, diagnostics);
    }
    if (loop->condition != nullptr) {
      const auto condition = check_expression(*loop->condition, symbols, diagnostics);
      if (condition.kind != TypeKind::Bool && !is_error(condition)) {
        report(diagnostics, "T002", "condition must have type bool", loop->condition->span);
      }
    }
    if (loop->update != nullptr) {
      check_statement(*loop->update, symbols, return_type, diagnostics);
    }
    check_statement(*loop->body, symbols, return_type, diagnostics);
    symbols.pop_scope();
    return false;
  }
  if (auto* result = dynamic_cast<ReturnStmt*>(&statement)) {
    const auto actual = result->value == nullptr ? Type::unit() : check_expression(*result->value, symbols, diagnostics);
    if (!is_error(actual) && !types_equal(actual, return_type)) {
      report(diagnostics, "T006", "return type mismatch: expected " + type_name(return_type) + ", found " +
                                      type_name(actual), result->span);
    }
    return true;
  }
  return false;
}

Type SemanticAnalyzer::type_from_annotation(const TypeAnnotation& annotation) const {
  return annotation.type;
}

void SemanticAnalyzer::report(Diagnostics& diagnostics, std::string code, std::string message, Span span) const {
  diagnostics.error(std::move(code), std::move(message), std::move(span));
}

}  // namespace kite

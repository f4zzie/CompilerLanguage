#include "parser/ast_printer.hpp"

#include <sstream>
#include <string_view>

namespace kite {
namespace {

std::string type_text(const Type& type) {
  switch (type.kind) {
    case TypeKind::Int:
      return "int";
    case TypeKind::Float:
      return "float";
    case TypeKind::Bool:
      return "bool";
    case TypeKind::String:
      return "string";
    case TypeKind::Unit:
      return "unit";
    case TypeKind::Array:
      return type.element == nullptr ? "[]" : type_text(*type.element) + "[]";
    case TypeKind::Error:
      return "<error>";
  }
  return "<error>";
}

std::string span_text(const Span& span) {
  std::ostringstream output;
  output << span.start.file << ':' << span.start.line << ':' << span.start.column << '-'
         << span.end.line << ':' << span.end.column;
  return output.str();
}

std::string quote(std::string_view text) {
  std::string result = "\"";
  for (const char character : text) {
    switch (character) {
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '"':
        result += "\\\"";
        break;
      case '\\':
        result += "\\\\";
        break;
      default:
        result.push_back(character);
        break;
    }
  }
  result += '"';
  return result;
}

const char* literal_kind_name(LiteralKind kind) {
  switch (kind) {
    case LiteralKind::Integer:
      return "integer";
    case LiteralKind::Float:
      return "float";
    case LiteralKind::String:
      return "string";
    case LiteralKind::Boolean:
      return "boolean";
    case LiteralKind::Unit:
      return "unit";
  }
  return "unknown";
}

const char* unary_name(UnaryOperator op) {
  return op == UnaryOperator::Negate ? "-" : "not";
}

const char* binary_name(BinaryOperator op) {
  switch (op) {
    case BinaryOperator::Add:
      return "+";
    case BinaryOperator::Subtract:
      return "-";
    case BinaryOperator::Multiply:
      return "*";
    case BinaryOperator::Divide:
      return "/";
    case BinaryOperator::Modulo:
      return "%";
    case BinaryOperator::Equal:
      return "==";
    case BinaryOperator::NotEqual:
      return "!=";
    case BinaryOperator::Less:
      return "<";
    case BinaryOperator::LessEqual:
      return "<=";
    case BinaryOperator::Greater:
      return ">";
    case BinaryOperator::GreaterEqual:
      return ">=";
    case BinaryOperator::And:
      return "and";
    case BinaryOperator::Or:
      return "or";
  }
  return "?";
}

void append_indent(std::ostringstream& output, std::size_t depth) {
  output << std::string(depth * 2, ' ');
}

template <typename Node>
void append_node_header(std::ostringstream& output, std::size_t depth, const char* name,
                        const Node& node, std::string_view details = {}) {
  append_indent(output, depth);
  output << name << '#' << node.id;
  if (!details.empty()) {
    output << ' ' << details;
  }
  output << " [" << span_text(node.span) << "]\n";
}

void append_expr(std::ostringstream& output, const Expr& expression, std::size_t depth);
void append_stmt(std::ostringstream& output, const Stmt& statement, std::size_t depth);

void append_expr(std::ostringstream& output, const Expr& expression, std::size_t depth) {
  if (const auto* literal = dynamic_cast<const LiteralExpr*>(&expression)) {
    std::ostringstream details;
    details << "kind=" << literal_kind_name(literal->kind) << " text=" << quote(literal->text);
    append_node_header(output, depth, "LiteralExpr", *literal, details.str());
  } else if (const auto* name = dynamic_cast<const NameExpr*>(&expression)) {
    append_node_header(output, depth, "NameExpr", *name, "name=" + name->name);
  } else if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
    append_node_header(output, depth, "UnaryExpr", *unary, "op=" + std::string(unary_name(unary->op)));
    append_expr(output, *unary->operand, depth + 1);
  } else if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
    append_node_header(output, depth, "BinaryExpr", *binary,
                       "op=" + std::string(binary_name(binary->op)));
    append_expr(output, *binary->left, depth + 1);
    append_expr(output, *binary->right, depth + 1);
  } else if (const auto* call = dynamic_cast<const CallExpr*>(&expression)) {
    append_node_header(output, depth, "CallExpr", *call, "callee=" + call->callee);
    for (const auto& argument : call->arguments) {
      append_expr(output, *argument, depth + 1);
    }
  } else if (const auto* array = dynamic_cast<const ArrayExpr*>(&expression)) {
    append_node_header(output, depth, "ArrayExpr", *array);
    for (const auto& element : array->elements) {
      append_expr(output, *element, depth + 1);
    }
  } else if (const auto* assignment = dynamic_cast<const AssignmentExpr*>(&expression)) {
    append_node_header(output, depth, "AssignmentExpr", *assignment);
    append_expr(output, *assignment->target, depth + 1);
    append_expr(output, *assignment->value, depth + 1);
  } else if (const auto* index = dynamic_cast<const IndexExpr*>(&expression)) {
    append_node_header(output, depth, "IndexExpr", *index);
    append_expr(output, *index->object, depth + 1);
    append_expr(output, *index->index, depth + 1);
  } else if (const auto* pipe = dynamic_cast<const PipeExpr*>(&expression)) {
    append_node_header(output, depth, "PipeExpr", *pipe, "function=" + pipe->function_name);
    append_expr(output, *pipe->value, depth + 1);
  }
}

void append_stmt(std::ostringstream& output, const Stmt& statement, std::size_t depth) {
  if (const auto* expression = dynamic_cast<const ExprStmt*>(&statement)) {
    append_node_header(output, depth, "ExprStmt", *expression);
    append_expr(output, *expression->expression, depth + 1);
  } else if (const auto* let = dynamic_cast<const LetStmt*>(&statement)) {
    append_node_header(output, depth, "LetStmt", *let, "name=" + let->name);
    if (let->annotation.has_value()) {
      append_indent(output, depth + 1);
      output << "TypeAnnotation type=" << type_text(let->annotation->type) << " ["
             << span_text(let->annotation->span) << "]\n";
    }
    append_expr(output, *let->initializer, depth + 1);
  } else if (const auto* assign = dynamic_cast<const AssignStmt*>(&statement)) {
    append_node_header(output, depth, "AssignStmt", *assign);
    append_expr(output, *assign->target, depth + 1);
    append_expr(output, *assign->value, depth + 1);
  } else if (const auto* block = dynamic_cast<const BlockStmt*>(&statement)) {
    append_node_header(output, depth, "BlockStmt", *block);
    for (const auto& child : block->statements) {
      append_stmt(output, *child, depth + 1);
    }
  } else if (const auto* conditional = dynamic_cast<const IfStmt*>(&statement)) {
    append_node_header(output, depth, "IfStmt", *conditional);
    append_expr(output, *conditional->condition, depth + 1);
    append_stmt(output, *conditional->then_branch, depth + 1);
    if (conditional->else_branch != nullptr) {
      append_stmt(output, *conditional->else_branch, depth + 1);
    }
  } else if (const auto* loop = dynamic_cast<const WhileStmt*>(&statement)) {
    append_node_header(output, depth, "WhileStmt", *loop);
    append_expr(output, *loop->condition, depth + 1);
    append_stmt(output, *loop->body, depth + 1);
  } else if (const auto* loop = dynamic_cast<const ForStmt*>(&statement)) {
    append_node_header(output, depth, "ForStmt", *loop);
    if (loop->initializer != nullptr) {
      append_stmt(output, *loop->initializer, depth + 1);
    }
    if (loop->condition != nullptr) {
      append_expr(output, *loop->condition, depth + 1);
    }
    if (loop->update != nullptr) {
      append_stmt(output, *loop->update, depth + 1);
    }
    append_stmt(output, *loop->body, depth + 1);
  } else if (const auto* result = dynamic_cast<const ReturnStmt*>(&statement)) {
    append_node_header(output, depth, "ReturnStmt", *result);
    if (result->value != nullptr) {
      append_expr(output, *result->value, depth + 1);
    }
  }
}

}  // namespace

std::string print_ast(const Program& program) {
  std::ostringstream output;
  append_node_header(output, 0, "Program", program);
  for (const auto& declaration : program.declarations) {
    if (const auto* function = dynamic_cast<const FunctionDecl*>(declaration.get())) {
      std::ostringstream details;
      details << "name=" << function->name << " return_type=" << type_text(function->return_type.type);
      append_node_header(output, 1, "FunctionDecl", *function, details.str());
      for (const auto& parameter : function->parameters) {
        append_indent(output, 2);
        output << "Parameter name=" << parameter.name << " type=" << type_text(parameter.annotation.type)
               << " [" << span_text(parameter.span) << "]\n";
      }
      append_stmt(output, *function->body, 2);
    }
  }
  for (const auto& statement : program.statements) {
    append_stmt(output, *statement, 1);
  }
  return output.str();
}

}  // namespace kite

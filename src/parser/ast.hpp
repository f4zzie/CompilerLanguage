#pragma once

#include "common/source.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace kite {

using NodeId = std::uint64_t;

enum class TypeKind { Int, Float, Bool, String, Unit, Array, Error };

struct Type {
  TypeKind kind{TypeKind::Error};
  std::shared_ptr<Type> element;

  static Type error() { return Type{TypeKind::Error, nullptr}; }
  static Type unit() { return Type{TypeKind::Unit, nullptr}; }
};

struct TypeAnnotation {
  Type type{Type::error()};
  Span span{Span::synthetic()};
};

struct AstNode {
  NodeId id{0};
  Span span{Span::synthetic()};
  virtual ~AstNode() = default;
};

struct Expr : AstNode {
  std::optional<Type> inferred_type;
  ~Expr() override = default;
};
using ExprPtr = std::unique_ptr<Expr>;

struct Stmt : AstNode {
  ~Stmt() override = default;
};
using StmtPtr = std::unique_ptr<Stmt>;

struct Decl : AstNode {
  ~Decl() override = default;
};
using DeclPtr = std::unique_ptr<Decl>;

enum class LiteralKind { Integer, Float, String, Boolean, Unit };
struct LiteralExpr final : Expr {
  LiteralKind kind{LiteralKind::Unit};
  std::string text;
};

enum class UnaryOperator { Negate, Not };
struct UnaryExpr final : Expr {
  UnaryOperator op{UnaryOperator::Negate};
  ExprPtr operand;
};

enum class BinaryOperator {
  Add,
  Subtract,
  Multiply,
  Divide,
  Modulo,
  Equal,
  NotEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  And,
  Or,
};
struct BinaryExpr final : Expr {
  BinaryOperator op{BinaryOperator::Add};
  ExprPtr left;
  ExprPtr right;
};

struct NameExpr final : Expr {
  std::string name;
};

struct CallExpr final : Expr {
  std::string callee;
  std::vector<ExprPtr> arguments;
};

struct ArrayExpr final : Expr {
  std::vector<ExprPtr> elements;
};

struct AssignmentExpr final : Expr {
  ExprPtr target;
  ExprPtr value;
};

struct IndexExpr final : Expr {
  ExprPtr object;
  ExprPtr index;
};

struct PipeExpr final : Expr {
  ExprPtr value;
  std::string function_name;
};

struct ExprStmt final : Stmt {
  ExprPtr expression;
};

struct LetStmt final : Stmt {
  std::string name;
  std::optional<TypeAnnotation> annotation;
  ExprPtr initializer;
  Type inferred_type{Type::error()};
};

struct AssignStmt final : Stmt {
  ExprPtr target;
  ExprPtr value;
};

struct BlockStmt final : Stmt {
  std::vector<StmtPtr> statements;
};

struct IfStmt final : Stmt {
  ExprPtr condition;
  std::unique_ptr<BlockStmt> then_branch;
  std::unique_ptr<Stmt> else_branch;
};

struct WhileStmt final : Stmt {
  ExprPtr condition;
  std::unique_ptr<BlockStmt> body;
};

struct ForStmt final : Stmt {
  StmtPtr initializer;
  ExprPtr condition;
  StmtPtr update;
  std::unique_ptr<BlockStmt> body;
};

struct ReturnStmt final : Stmt {
  ExprPtr value;
};

struct Parameter {
  std::string name;
  TypeAnnotation annotation;
  Span span{Span::synthetic()};
};

struct FunctionDecl final : Decl {
  std::string name;
  std::vector<Parameter> parameters;
  TypeAnnotation return_type;
  std::unique_ptr<BlockStmt> body;
};

struct Program final : AstNode {
  std::vector<DeclPtr> declarations;
  std::vector<StmtPtr> statements;
};

}  // namespace kite

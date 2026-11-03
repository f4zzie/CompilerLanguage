#pragma once

#include "diagnostics/diagnostic.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "parser/precedence.hpp"

#include <memory>
#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace kite {

class Parser {
 public:
  struct Options {
    std::size_t max_depth{200};
  };

  Parser() = default;
  explicit Parser(Options options) : options_(options) {
    if (options_.max_depth == 0) {
      options_.max_depth = 1;
    }
  }

  std::unique_ptr<Program> parse(const std::vector<Token>& tokens, Diagnostics& diagnostics);
  ExprPtr parse_expression_for_test(const std::vector<Token>& tokens, Diagnostics& diagnostics);

 private:
  ExprPtr parse_expression(int minimum_precedence);
  ExprPtr parse_unary();
  ExprPtr parse_postfix();
  ExprPtr parse_primary();
  ExprPtr parse_array_literal();
  ExprPtr parse_assignment_expression();
  std::unique_ptr<FunctionDecl> parse_function_declaration();
  StmtPtr parse_statement();
  StmtPtr parse_block();
  StmtPtr parse_let_statement(bool require_semicolon);
  StmtPtr parse_if_statement();
  StmtPtr parse_while_statement();
  StmtPtr parse_for_statement();
  StmtPtr parse_return_statement();
  StmtPtr parse_expression_statement(bool require_semicolon);
  TypeAnnotation parse_type_annotation();
  Parameter parse_parameter();
  void synchronize_statement();
  bool enter_depth(Span span);
  void leave_depth();
  Span skip_balanced(TokenKind open, TokenKind close);
  void skip_expression_tail();
  ExprPtr depth_error(Span span);
  void record_expression_depth(const Expr& expression, std::size_t depth);
  std::size_t expression_depth(const Expr& expression) const;
  bool check(TokenKind kind) const;
  bool match(TokenKind kind);
  const Token& current() const;
  const Token& previous() const;
  const Token& consume(TokenKind kind, std::string_view expectation);
  void report(std::string code, std::string message, Span span);
  static int precedence(TokenKind kind);
  static BinaryOperator binary_operator(TokenKind kind);
  static bool is_assignable(const Expr& expression);

  const std::vector<Token>* tokens_{nullptr};
  Diagnostics* diagnostics_{nullptr};
  Options options_;
  std::size_t current_index_{0};
  NodeId next_node_id_{1};
  std::size_t parse_depth_{0};
  std::unordered_map<NodeId, std::size_t> expression_depths_;
};

}  // namespace kite

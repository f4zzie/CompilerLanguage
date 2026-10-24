#include "parser/parser.hpp"

#include <utility>

namespace kite {

std::unique_ptr<Program> Parser::parse(const std::vector<Token>& tokens, Diagnostics& diagnostics) {
  tokens_ = &tokens;
  diagnostics_ = &diagnostics;
  current_index_ = 0;
  next_node_id_ = 1;
  auto program = std::make_unique<Program>();
  program->id = 0;
  if (!tokens.empty()) {
    program->span = Span{tokens.front().span.start, tokens.back().span.end};
  }
  while (!check(TokenKind::Eof)) {
    try {
      if (match(TokenKind::Fn)) {
        auto function = parse_function_declaration();
        if (function != nullptr) {
          program->declarations.push_back(std::move(function));
        }
      } else {
        auto statement = parse_statement();
        if (statement != nullptr) {
          program->statements.push_back(std::move(statement));
        }
      }
    } catch (...) {
      synchronize_statement();
    }
  }
  return program;
}

ExprPtr Parser::parse_expression_for_test(const std::vector<Token>& tokens,
                                           Diagnostics& diagnostics) {
  tokens_ = &tokens;
  diagnostics_ = &diagnostics;
  current_index_ = 0;
  next_node_id_ = 1;
  auto expression = parse_assignment_expression();
  if (!check(TokenKind::Eof) && !check(TokenKind::Semicolon)) {
    report("P001", "unexpected token " + std::string(token_kind_name(current().kind)), current().span);
  }
  return expression;
}

ExprPtr Parser::parse_assignment_expression() {
  auto left = parse_expression(static_cast<int>(Precedence::Pipe));
  if (!match(TokenKind::Equal)) {
    return left;
  }

  const auto equal_span = previous().span;
  auto right = parse_assignment_expression();
  if (!is_assignable(*left)) {
    report("P003", "invalid assignment target", left->span);
  }
  auto assignment = std::make_unique<AssignmentExpr>();
  assignment->id = next_node_id_++;
  assignment->span = Span{left->span.start, right->span.end};
  assignment->target = std::move(left);
  assignment->value = std::move(right);
  (void)equal_span;
  return assignment;
}

ExprPtr Parser::parse_expression(int minimum_precedence) {
  auto left = parse_unary();
  while (true) {
    const int operator_precedence = precedence(current().kind);
    if (operator_precedence < minimum_precedence) {
      break;
    }
    const auto operator_kind = current().kind;
    const auto operator_span = current().span;
    ++current_index_;
    if (operator_kind == TokenKind::Pipe) {
      const auto& function = consume(TokenKind::Identifier, "function name after |> ");
      auto pipe = std::make_unique<PipeExpr>();
      pipe->id = next_node_id_++;
      pipe->span = Span{left->span.start, function.span.end};
      pipe->value = std::move(left);
      pipe->function_name = function.lexeme;
      left = std::move(pipe);
      continue;
    }

    auto right = parse_expression(operator_precedence + 1);
    auto binary = std::make_unique<BinaryExpr>();
    binary->id = next_node_id_++;
    binary->span = Span{left->span.start, right->span.end};
    binary->op = binary_operator(operator_kind);
    binary->left = std::move(left);
    binary->right = std::move(right);
    left = std::move(binary);
    (void)operator_span;
  }
  return left;
}

ExprPtr Parser::parse_unary() {
  if (match(TokenKind::Minus) || match(TokenKind::Not)) {
    const auto operator_token = previous();
    auto unary = std::make_unique<UnaryExpr>();
    unary->id = next_node_id_++;
    unary->op = operator_token.kind == TokenKind::Minus ? UnaryOperator::Negate : UnaryOperator::Not;
    unary->operand = parse_unary();
    unary->span = Span{operator_token.span.start, unary->operand->span.end};
    return unary;
  }
  return parse_postfix();
}

ExprPtr Parser::parse_postfix() {
  auto expression = parse_primary();
  while (true) {
    if (match(TokenKind::LeftParen)) {
      std::string callee;
      if (const auto* name = dynamic_cast<const NameExpr*>(expression.get())) {
        callee = name->name;
      } else {
        report("P003", "invalid call target", expression->span);
      }
      auto call = std::make_unique<CallExpr>();
      call->id = next_node_id_++;
      call->callee = std::move(callee);
      if (!check(TokenKind::RightParen)) {
        do {
          call->arguments.push_back(parse_assignment_expression());
        } while (match(TokenKind::Comma));
      }
      const auto right_paren = consume(TokenKind::RightParen, "')'");
      call->span = Span{expression->span.start, right_paren.span.end};
      expression = std::move(call);
      continue;
    }
    if (match(TokenKind::LeftBracket)) {
      auto index = std::make_unique<IndexExpr>();
      index->id = next_node_id_++;
      index->object = std::move(expression);
      index->index = parse_assignment_expression();
      const auto right_bracket = consume(TokenKind::RightBracket, "']'");
      index->span = Span{index->object->span.start, right_bracket.span.end};
      expression = std::move(index);
      continue;
    }
    break;
  }
  return expression;
}

ExprPtr Parser::parse_primary() {
  const Token token = current();
  if (match(TokenKind::Integer) || match(TokenKind::Float) || match(TokenKind::String) ||
      match(TokenKind::True) || match(TokenKind::False) || match(TokenKind::Unit)) {
    auto literal = std::make_unique<LiteralExpr>();
    literal->id = next_node_id_++;
    literal->span = token.span;
    literal->text = token.lexeme;
    if (token.kind == TokenKind::Integer) {
      literal->kind = LiteralKind::Integer;
    } else if (token.kind == TokenKind::Float) {
      literal->kind = LiteralKind::Float;
    } else if (token.kind == TokenKind::String) {
      literal->kind = LiteralKind::String;
    } else if (token.kind == TokenKind::True || token.kind == TokenKind::False) {
      literal->kind = LiteralKind::Boolean;
    } else {
      literal->kind = LiteralKind::Unit;
    }
    return literal;
  }

  if (match(TokenKind::Identifier)) {
    auto name = std::make_unique<NameExpr>();
    name->id = next_node_id_++;
    name->span = token.span;
    name->name = token.lexeme;
    return name;
  }

  if (match(TokenKind::LeftParen)) {
    auto expression = parse_assignment_expression();
    const auto right_paren = consume(TokenKind::RightParen, "')'");
    expression->span = Span{token.span.start, right_paren.span.end};
    return expression;
  }

  if (match(TokenKind::LeftBracket)) {
    return parse_array_literal();
  }

  report("P001", "unexpected token " + std::string(token_kind_name(token.kind)), token.span);
  auto error = std::make_unique<LiteralExpr>();
  error->id = next_node_id_++;
  error->kind = LiteralKind::Unit;
  error->text = "unit";
  error->span = token.span;
  if (!check(TokenKind::Eof)) {
    ++current_index_;
  }
  return error;
}

ExprPtr Parser::parse_array_literal() {
  auto array = std::make_unique<ArrayExpr>();
  array->id = next_node_id_++;
  const auto left_bracket = previous();
  if (!check(TokenKind::RightBracket)) {
    do {
      array->elements.push_back(parse_assignment_expression());
    } while (match(TokenKind::Comma));
  }
  const auto right_bracket = consume(TokenKind::RightBracket, "']'");
  array->span = Span{left_bracket.span.start, right_bracket.span.end};
  return array;
}

std::unique_ptr<FunctionDecl> Parser::parse_function_declaration() {
  const auto function_start = previous().span.start;
  const auto name = consume(TokenKind::Identifier, "function name");
  auto function = std::make_unique<FunctionDecl>();
  function->id = next_node_id_++;
  function->name = name.lexeme;
  consume(TokenKind::LeftParen, "'('");
  if (!check(TokenKind::RightParen)) {
    do {
      function->parameters.push_back(parse_parameter());
    } while (match(TokenKind::Comma));
  }
  consume(TokenKind::RightParen, "')'");
  consume(TokenKind::Arrow, "'->'");
  function->return_type = parse_type_annotation();
  auto body = parse_block();
  function->body = std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>(body.release()));
  function->span = Span{function_start, function->body->span.end};
  return function;
}

StmtPtr Parser::parse_statement() {
  if (match(TokenKind::Let)) {
    return parse_let_statement(true);
  }
  if (match(TokenKind::If)) {
    return parse_if_statement();
  }
  if (match(TokenKind::While)) {
    return parse_while_statement();
  }
  if (match(TokenKind::For)) {
    return parse_for_statement();
  }
  if (match(TokenKind::Return)) {
    return parse_return_statement();
  }
  if (check(TokenKind::LeftBrace)) {
    return parse_block();
  }
  return parse_expression_statement(true);
}

StmtPtr Parser::parse_block() {
  const auto left_brace = consume(TokenKind::LeftBrace, "'{'");
  auto block = std::make_unique<BlockStmt>();
  block->id = next_node_id_++;
  while (!check(TokenKind::RightBrace) && !check(TokenKind::Eof)) {
    try {
      auto statement = parse_statement();
      if (statement != nullptr) {
        block->statements.push_back(std::move(statement));
      }
    } catch (...) {
      synchronize_statement();
    }
  }
  const auto right_brace = consume(TokenKind::RightBrace, "'}'");
  block->span = Span{left_brace.span.start, right_brace.span.end};
  return block;
}

StmtPtr Parser::parse_let_statement(bool require_semicolon) {
  const auto start = previous().span.start;
  const auto name = consume(TokenKind::Identifier, "variable name");
  auto statement = std::make_unique<LetStmt>();
  statement->id = next_node_id_++;
  statement->name = name.lexeme;
  if (match(TokenKind::Colon)) {
    statement->annotation = parse_type_annotation();
  }
  consume(TokenKind::Equal, "'='");
  statement->initializer = parse_assignment_expression();
  const auto end = require_semicolon ? consume(TokenKind::Semicolon, "';'").span.end
                                     : statement->initializer->span.end;
  statement->span = Span{start, end};
  return statement;
}

StmtPtr Parser::parse_if_statement() {
  const auto start = previous().span.start;
  consume(TokenKind::LeftParen, "'('");
  auto statement = std::make_unique<IfStmt>();
  statement->id = next_node_id_++;
  statement->condition = parse_assignment_expression();
  consume(TokenKind::RightParen, "')'");
  auto then_branch = parse_block();
  statement->then_branch = std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>(then_branch.release()));
  if (match(TokenKind::Else)) {
    statement->else_branch = check(TokenKind::If) ? parse_if_statement() : parse_block();
  }
  statement->span = Span{start, statement->else_branch != nullptr ? statement->else_branch->span.end
                                                                    : statement->then_branch->span.end};
  return statement;
}

StmtPtr Parser::parse_while_statement() {
  const auto start = previous().span.start;
  consume(TokenKind::LeftParen, "'('");
  auto statement = std::make_unique<WhileStmt>();
  statement->id = next_node_id_++;
  statement->condition = parse_assignment_expression();
  consume(TokenKind::RightParen, "')'");
  auto body = parse_block();
  statement->body = std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>(body.release()));
  statement->span = Span{start, statement->body->span.end};
  return statement;
}

StmtPtr Parser::parse_for_statement() {
  const auto start = previous().span.start;
  consume(TokenKind::LeftParen, "'('");
  auto statement = std::make_unique<ForStmt>();
  statement->id = next_node_id_++;
  if (!check(TokenKind::Semicolon)) {
    statement->initializer = check(TokenKind::Let) ? (match(TokenKind::Let), parse_let_statement(false))
                                                   : parse_expression_statement(false);
  }
  consume(TokenKind::Semicolon, "';'");
  if (!check(TokenKind::Semicolon)) {
    statement->condition = parse_assignment_expression();
  }
  consume(TokenKind::Semicolon, "';'");
  if (!check(TokenKind::RightParen)) {
    statement->update = parse_expression_statement(false);
  }
  consume(TokenKind::RightParen, "')'");
  auto body = parse_block();
  statement->body = std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>(body.release()));
  statement->span = Span{start, statement->body->span.end};
  return statement;
}

StmtPtr Parser::parse_return_statement() {
  const auto start = previous().span.start;
  auto statement = std::make_unique<ReturnStmt>();
  statement->id = next_node_id_++;
  if (!check(TokenKind::Semicolon)) {
    statement->value = parse_assignment_expression();
  }
  const auto semicolon = consume(TokenKind::Semicolon, "';'");
  statement->span = Span{start, semicolon.span.end};
  return statement;
}

StmtPtr Parser::parse_expression_statement(bool require_semicolon) {
  auto expression = parse_assignment_expression();
  auto statement = std::make_unique<ExprStmt>();
  statement->id = next_node_id_++;
  statement->span = expression->span;
  statement->expression = std::move(expression);
  if (require_semicolon) {
    statement->span.end = consume(TokenKind::Semicolon, "';'").span.end;
  }
  return statement;
}

TypeAnnotation Parser::parse_type_annotation() {
  const auto start = current().span.start;
  Type type{TypeKind::Error, nullptr};
  switch (current().kind) {
    case TokenKind::IntType:
      type.kind = TypeKind::Int;
      break;
    case TokenKind::FloatType:
      type.kind = TypeKind::Float;
      break;
    case TokenKind::BoolType:
      type.kind = TypeKind::Bool;
      break;
    case TokenKind::StringType:
      type.kind = TypeKind::String;
      break;
    case TokenKind::Unit:
    case TokenKind::UnitType:
      type.kind = TypeKind::Unit;
      break;
    default:
      report("P002", "expected type", current().span);
      return TypeAnnotation{Type::error(), current().span};
  }
  const auto base = current();
  ++current_index_;
  auto end = base.span.end;
  while (match(TokenKind::LeftBracket)) {
    const auto right = consume(TokenKind::RightBracket, "']'");
    type = Type{TypeKind::Array, std::make_shared<Type>(type)};
    end = right.span.end;
  }
  return TypeAnnotation{type, Span{start, end}};
}

Parameter Parser::parse_parameter() {
  const auto start = current().span.start;
  const auto name = consume(TokenKind::Identifier, "parameter name");
  consume(TokenKind::Colon, "':'");
  auto annotation = parse_type_annotation();
  return Parameter{name.lexeme, annotation, Span{start, annotation.span.end}};
}

void Parser::synchronize_statement() {
  while (!check(TokenKind::Eof) && !check(TokenKind::Semicolon) && !check(TokenKind::RightBrace) &&
         !check(TokenKind::Fn) && !check(TokenKind::Let) && !check(TokenKind::If) &&
         !check(TokenKind::While) && !check(TokenKind::For) && !check(TokenKind::Return)) {
    ++current_index_;
  }
  if (check(TokenKind::Semicolon)) {
    ++current_index_;
  }
}

bool Parser::check(TokenKind kind) const {
  return current().kind == kind;
}

bool Parser::match(TokenKind kind) {
  if (!check(kind)) {
    return false;
  }
  ++current_index_;
  return true;
}

const Token& Parser::current() const {
  static const Token eof{TokenKind::Eof, "", 0, 0.0, Span::synthetic()};
  if (tokens_ == nullptr || tokens_->empty()) {
    return eof;
  }
  return current_index_ < tokens_->size() ? (*tokens_)[current_index_] : tokens_->back();
}

const Token& Parser::previous() const {
  static const Token eof{TokenKind::Eof, "", 0, 0.0, Span::synthetic()};
  if (tokens_ == nullptr || tokens_->empty() || current_index_ == 0) {
    return eof;
  }
  return (*tokens_)[current_index_ - 1];
}

const Token& Parser::consume(TokenKind kind, std::string_view expectation) {
  if (check(kind)) {
    const auto& token = current();
    ++current_index_;
    return token;
  }
  report("P002", "expected " + std::string(expectation), current().span);
  return current();
}

void Parser::report(std::string code, std::string message, Span span) {
  if (diagnostics_ != nullptr) {
    diagnostics_->error(std::move(code), std::move(message), std::move(span));
  }
}

int Parser::precedence(TokenKind kind) {
  switch (kind) {
    case TokenKind::Pipe:
      return static_cast<int>(Precedence::Pipe);
    case TokenKind::Or:
      return static_cast<int>(Precedence::Or);
    case TokenKind::And:
      return static_cast<int>(Precedence::And);
    case TokenKind::EqualEqual:
    case TokenKind::BangEqual:
      return static_cast<int>(Precedence::Equality);
    case TokenKind::Less:
    case TokenKind::LessEqual:
    case TokenKind::Greater:
    case TokenKind::GreaterEqual:
      return static_cast<int>(Precedence::Comparison);
    case TokenKind::Plus:
    case TokenKind::Minus:
      return static_cast<int>(Precedence::Term);
    case TokenKind::Star:
    case TokenKind::Slash:
    case TokenKind::Percent:
      return static_cast<int>(Precedence::Factor);
    default:
      return 0;
  }
}

BinaryOperator Parser::binary_operator(TokenKind kind) {
  switch (kind) {
    case TokenKind::Plus:
      return BinaryOperator::Add;
    case TokenKind::Minus:
      return BinaryOperator::Subtract;
    case TokenKind::Star:
      return BinaryOperator::Multiply;
    case TokenKind::Slash:
      return BinaryOperator::Divide;
    case TokenKind::Percent:
      return BinaryOperator::Modulo;
    case TokenKind::EqualEqual:
      return BinaryOperator::Equal;
    case TokenKind::BangEqual:
      return BinaryOperator::NotEqual;
    case TokenKind::Less:
      return BinaryOperator::Less;
    case TokenKind::LessEqual:
      return BinaryOperator::LessEqual;
    case TokenKind::Greater:
      return BinaryOperator::Greater;
    case TokenKind::GreaterEqual:
      return BinaryOperator::GreaterEqual;
    case TokenKind::And:
      return BinaryOperator::And;
    case TokenKind::Or:
      return BinaryOperator::Or;
    default:
      return BinaryOperator::Add;
  }
}

bool Parser::is_assignable(const Expr& expression) {
  return dynamic_cast<const NameExpr*>(&expression) != nullptr ||
         dynamic_cast<const IndexExpr*>(&expression) != nullptr;
}

}  // namespace kite

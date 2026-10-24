#include "doctest/doctest.h"

#include "parser/ast.hpp"
#include "parser/ast_printer.hpp"

#include <memory>

TEST_CASE("ast printer prints literals and spans") {
  kite::Program program;
  auto statement = std::make_unique<kite::ExprStmt>();
  statement->id = 2;
  statement->span = kite::Span{
      kite::SourceLocation{"sample.kite", 1, 1, 0},
      kite::SourceLocation{"sample.kite", 1, 3, 2},
  };
  auto literal = std::make_unique<kite::LiteralExpr>();
  literal->id = 3;
  literal->kind = kite::LiteralKind::Integer;
  literal->text = "42";
  literal->span = kite::Span{
      kite::SourceLocation{"sample.kite", 1, 1, 0},
      kite::SourceLocation{"sample.kite", 1, 3, 2},
  };
  statement->expression = std::move(literal);
  program.statements.push_back(std::move(statement));

  CHECK(kite::print_ast(program) ==
        "Program#0 [<synthetic>:1:1-1:1]\n"
        "  ExprStmt#2 [sample.kite:1:1-1:3]\n"
        "    LiteralExpr#3 kind=integer text=\"42\" [sample.kite:1:1-1:3]\n");
}

TEST_CASE("ast printer prints function declarations and parameters") {
  kite::Program program;
  auto function = std::make_unique<kite::FunctionDecl>();
  function->id = 5;
  function->name = "add";
  function->parameters.push_back(kite::Parameter{
      "left",
      kite::TypeAnnotation{kite::Type{kite::TypeKind::Int, nullptr}, kite::Span::synthetic()},
      kite::Span::synthetic(),
  });
  function->return_type = kite::TypeAnnotation{kite::Type{kite::TypeKind::Int, nullptr},
                                                kite::Span::synthetic()};
  function->body = std::make_unique<kite::BlockStmt>();
  program.declarations.push_back(std::move(function));

  const auto output = kite::print_ast(program);
  CHECK(output.find("FunctionDecl#5 name=add") != std::string::npos);
  CHECK(output.find("Parameter name=left type=int") != std::string::npos);
  CHECK(output.find("return_type=int") != std::string::npos);
}

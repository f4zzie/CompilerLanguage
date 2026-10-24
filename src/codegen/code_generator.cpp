#include "codegen/code_generator.hpp"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace kite {
namespace {

class Emitter {
 public:
  explicit Emitter(Diagnostics& diagnostics) : diagnostics_(diagnostics) {}

  BytecodeModule generate(const Program& program) {
    module_.functions.push_back(FunctionInfo{"__main", 0, 0, 0});
    function_indices_["__main"] = 0;
    for (const char* builtin : {"print", "input", "len", "to_string", "to_int"}) {
      function_indices_[builtin] = module_.functions.size();
      module_.functions.push_back(FunctionInfo{builtin, 0, 0, 0});
    }
    for (const auto& declaration : program.declarations) {
      const auto* function = dynamic_cast<const FunctionDecl*>(declaration.get());
      if (function == nullptr || function_indices_.contains(function->name)) {
        continue;
      }
      function_indices_[function->name] = module_.functions.size();
      module_.functions.push_back(FunctionInfo{function->name, 0, function->parameters.size(), 0});
    }
    for (const auto& statement : program.statements) {
      if (const auto* let = dynamic_cast<const LetStmt*>(statement.get())) {
        if (!global_indices_.contains(let->name)) {
          global_indices_[let->name] = module_.global_count++;
        }
      }
    }

    begin_function(0, 0);
    for (const auto& statement : program.statements) {
      emit_statement(*statement);
    }
    emit(OpCode::Halt, 0, program.span);
    module_.functions[0].local_count = current_local_count_;

    for (const auto& declaration : program.declarations) {
      const auto* function = dynamic_cast<const FunctionDecl*>(declaration.get());
      if (function != nullptr) {
        emit_function(*function);
      }
    }
    return std::move(module_);
  }

 private:
  void begin_function(std::size_t function_index, std::size_t arity) {
    current_function_ = function_index;
    current_local_count_ = arity;
    local_scopes_.clear();
    local_scopes_.emplace_back();
    for (std::size_t index = 0; index < arity; ++index) {
      local_scopes_.back()["$param" + std::to_string(index)] = index;
    }
    module_.functions[function_index].entry = module_.instructions.size();
  }

  void emit_function(const FunctionDecl& function) {
    const auto function_index = function_indices_.at(function.name);
    begin_function(function_index, function.parameters.size());
    local_scopes_.front().clear();
    for (std::size_t index = 0; index < function.parameters.size(); ++index) {
      local_scopes_.front()[function.parameters[index].name] = index;
    }
    for (const auto& statement : function.body->statements) {
      emit_statement(*statement);
    }
    emit(OpCode::Unit, 0, function.span);
    emit(OpCode::Return, 0, function.span);
    module_.functions[function_index].local_count = current_local_count_;
  }

  void emit_statement(const Stmt& statement) {
    if (const auto* expression = dynamic_cast<const ExprStmt*>(&statement)) {
      emit_expression(*expression->expression);
      emit(OpCode::Pop, 0, expression->span);
      return;
    }
    if (const auto* let = dynamic_cast<const LetStmt*>(&statement)) {
      emit_expression(*let->initializer);
      if (current_function_ == 0 && local_scopes_.size() == 1) {
        const auto slot = global_indices_.at(let->name);
        emit(OpCode::StoreGlobal, static_cast<std::int32_t>(slot), let->span);
      } else {
        const auto slot = allocate_local(let->name);
        emit(OpCode::StoreLocal, static_cast<std::int32_t>(slot), let->span);
      }
      emit(OpCode::Pop, 0, let->span);
      return;
    }
    if (const auto* block = dynamic_cast<const BlockStmt*>(&statement)) {
      local_scopes_.emplace_back();
      for (const auto& child : block->statements) {
        emit_statement(*child);
      }
      local_scopes_.pop_back();
      return;
    }
    if (const auto* conditional = dynamic_cast<const IfStmt*>(&statement)) {
      emit_expression(*conditional->condition);
      const auto false_jump = emit(OpCode::JumpIfFalse, -1, conditional->condition->span);
      emit_statement(*conditional->then_branch);
      if (conditional->else_branch != nullptr) {
        const auto end_jump = emit(OpCode::Jump, -1, conditional->span);
        patch(false_jump, module_.instructions.size());
        emit_statement(*conditional->else_branch);
        patch(end_jump, module_.instructions.size());
      } else {
        patch(false_jump, module_.instructions.size());
      }
      return;
    }
    if (const auto* loop = dynamic_cast<const WhileStmt*>(&statement)) {
      const auto loop_start = module_.instructions.size();
      emit_expression(*loop->condition);
      const auto exit_jump = emit(OpCode::JumpIfFalse, -1, loop->condition->span);
      emit_statement(*loop->body);
      emit(OpCode::Jump, static_cast<std::int32_t>(loop_start), loop->span);
      patch(exit_jump, module_.instructions.size());
      return;
    }
    if (const auto* loop = dynamic_cast<const ForStmt*>(&statement)) {
      local_scopes_.emplace_back();
      if (loop->initializer != nullptr) {
        emit_statement(*loop->initializer);
      }
      const auto loop_start = module_.instructions.size();
      if (loop->condition != nullptr) {
        emit_expression(*loop->condition);
      } else {
        emit(OpCode::Constant, add_constant(std::int64_t{1}), loop->span);
      }
      const auto exit_jump = emit(OpCode::JumpIfFalse, -1, loop->span);
      emit_statement(*loop->body);
      if (loop->update != nullptr) {
        emit_statement(*loop->update);
      }
      emit(OpCode::Jump, static_cast<std::int32_t>(loop_start), loop->span);
      patch(exit_jump, module_.instructions.size());
      local_scopes_.pop_back();
      return;
    }
    if (const auto* result = dynamic_cast<const ReturnStmt*>(&statement)) {
      if (result->value == nullptr) {
        emit(OpCode::Unit, 0, result->span);
      } else {
        emit_expression(*result->value);
      }
      emit(OpCode::Return, 0, result->span);
      return;
    }
  }

  void emit_expression(const Expr& expression) {
    if (const auto* literal = dynamic_cast<const LiteralExpr*>(&expression)) {
      switch (literal->kind) {
        case LiteralKind::Integer:
          emit(OpCode::Constant, add_constant(std::stoll(literal->text)), literal->span);
          break;
        case LiteralKind::Float:
          emit(OpCode::Constant, add_constant(std::stod(literal->text)), literal->span);
          break;
        case LiteralKind::String:
          emit(OpCode::Constant, add_constant(literal->text), literal->span);
          break;
        case LiteralKind::Boolean:
          emit(OpCode::Constant, add_constant(literal->text == "true"), literal->span);
          break;
        case LiteralKind::Unit:
          emit(OpCode::Unit, 0, literal->span);
          break;
      }
      return;
    }
    if (const auto* name = dynamic_cast<const NameExpr*>(&expression)) {
      if (const auto slot = find_local(name->name); slot.has_value()) {
        emit(OpCode::LoadLocal, static_cast<std::int32_t>(*slot), name->span);
      } else if (global_indices_.contains(name->name)) {
        emit(OpCode::LoadGlobal, static_cast<std::int32_t>(global_indices_.at(name->name)), name->span);
      }
      return;
    }
    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
      emit_expression(*unary->operand);
      emit(unary->op == UnaryOperator::Negate ? OpCode::Negate : OpCode::Not, 0, unary->span);
      return;
    }
    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
      if (binary->op == BinaryOperator::And || binary->op == BinaryOperator::Or) {
        emit_expression(*binary->left);
        const auto branch = emit(OpCode::JumpIfFalse, -1, binary->span);
        if (binary->op == BinaryOperator::And) {
          emit_expression(*binary->right);
          const auto end = emit(OpCode::Jump, -1, binary->span);
          patch(branch, module_.instructions.size());
          emit(OpCode::Constant, add_constant(false), binary->span);
          patch(end, module_.instructions.size());
        } else {
          emit(OpCode::Constant, add_constant(true), binary->span);
          const auto end = emit(OpCode::Jump, -1, binary->span);
          patch(branch, module_.instructions.size());
          emit_expression(*binary->right);
          patch(end, module_.instructions.size());
        }
        return;
      }
      emit_expression(*binary->left);
      emit_expression(*binary->right);
      switch (binary->op) {
        case BinaryOperator::Add:
          emit(OpCode::Add, 0, binary->span);
          break;
        case BinaryOperator::Subtract:
          emit(OpCode::Subtract, 0, binary->span);
          break;
        case BinaryOperator::Multiply:
          emit(OpCode::Multiply, 0, binary->span);
          break;
        case BinaryOperator::Divide:
          emit(OpCode::Divide, 0, binary->span);
          break;
        case BinaryOperator::Modulo:
          emit(OpCode::Modulo, 0, binary->span);
          break;
        case BinaryOperator::Equal:
          emit(OpCode::Equal, 0, binary->span);
          break;
        case BinaryOperator::NotEqual:
          emit(OpCode::Equal, 0, binary->span);
          emit(OpCode::Not, 0, binary->span);
          break;
        case BinaryOperator::Less:
          emit(OpCode::Less, 0, binary->span);
          break;
        case BinaryOperator::LessEqual:
          emit(OpCode::Greater, 0, binary->span);
          emit(OpCode::Not, 0, binary->span);
          break;
        case BinaryOperator::Greater:
          emit(OpCode::Greater, 0, binary->span);
          break;
        case BinaryOperator::GreaterEqual:
          emit(OpCode::Less, 0, binary->span);
          emit(OpCode::Not, 0, binary->span);
          break;
        case BinaryOperator::And:
        case BinaryOperator::Or:
          break;
      }
      return;
    }
    if (const auto* assignment = dynamic_cast<const AssignmentExpr*>(&expression)) {
      emit_expression(*assignment->value);
      if (const auto* name = dynamic_cast<const NameExpr*>(assignment->target.get())) {
        if (const auto slot = find_local(name->name); slot.has_value()) {
          emit(OpCode::StoreLocal, static_cast<std::int32_t>(*slot), assignment->span);
        } else if (global_indices_.contains(name->name)) {
          emit(OpCode::StoreGlobal, static_cast<std::int32_t>(global_indices_.at(name->name)), assignment->span);
        }
      }
      return;
    }
    if (const auto* call = dynamic_cast<const CallExpr*>(&expression)) {
      for (const auto& argument : call->arguments) {
        emit_expression(*argument);
      }
      const auto iterator = function_indices_.find(call->callee);
      if (iterator == function_indices_.end()) {
        diagnostics_.error("S002", "unresolved name '" + call->callee + "'", call->span);
        emit(OpCode::Unit, 0, call->span);
      } else {
        emit(OpCode::Call, static_cast<std::int32_t>(iterator->second), call->span);
      }
      return;
    }
    if (const auto* pipe = dynamic_cast<const PipeExpr*>(&expression)) {
      emit_expression(*pipe->value);
      const auto iterator = function_indices_.find(pipe->function_name);
      if (iterator != function_indices_.end()) {
        emit(OpCode::Call, static_cast<std::int32_t>(iterator->second), pipe->span);
      }
      return;
    }
    if (const auto* index = dynamic_cast<const IndexExpr*>(&expression)) {
      emit_expression(*index->object);
      emit_expression(*index->index);
      emit(OpCode::Index, 0, index->span);
      return;
    }
    if (const auto* array = dynamic_cast<const ArrayExpr*>(&expression)) {
      for (const auto& element : array->elements) {
        emit_expression(*element);
      }
      emit(OpCode::MakeArray, static_cast<std::int32_t>(array->elements.size()), array->span);
    }
  }

  std::int32_t add_constant(Constant constant) {
    module_.constants.push_back(std::move(constant));
    return static_cast<std::int32_t>(module_.constants.size() - 1);
  }

  std::size_t allocate_local(const std::string& name) {
    const auto slot = current_local_count_++;
    local_scopes_.back()[name] = slot;
    return slot;
  }

  std::optional<std::size_t> find_local(const std::string& name) const {
    for (auto iterator = local_scopes_.rbegin(); iterator != local_scopes_.rend(); ++iterator) {
      const auto found = iterator->find(name);
      if (found != iterator->end()) {
        return found->second;
      }
    }
    return std::nullopt;
  }

  std::size_t emit(OpCode opcode, std::int32_t operand, Span span) {
    module_.instructions.push_back(Instruction{opcode, operand, std::move(span)});
    return module_.instructions.size() - 1;
  }

  void patch(std::size_t index, std::size_t target) {
    module_.instructions[index].operand = static_cast<std::int32_t>(target);
  }

  Diagnostics& diagnostics_;
  BytecodeModule module_;
  std::unordered_map<std::string, std::size_t> function_indices_;
  std::unordered_map<std::string, std::size_t> global_indices_;
  std::vector<std::unordered_map<std::string, std::size_t>> local_scopes_;
  std::size_t current_function_{0};
  std::size_t current_local_count_{0};
};

}  // namespace

BytecodeModule CodeGenerator::generate(const Program& program, Diagnostics& diagnostics) {
  return Emitter(diagnostics).generate(program);
}

}  // namespace kite

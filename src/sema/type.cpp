#include "sema/type.hpp"

namespace kite {

std::string type_name(const Type& type) {
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
      return type.element == nullptr ? "[]" : type_name(*type.element) + "[]";
    case TypeKind::Error:
      return "<error>";
  }
  return "<error>";
}

bool types_equal(const Type& left, const Type& right) {
  if (left.kind != right.kind) {
    return false;
  }
  if (left.kind != TypeKind::Array) {
    return true;
  }
  return left.element != nullptr && right.element != nullptr && types_equal(*left.element, *right.element);
}

}  // namespace kite

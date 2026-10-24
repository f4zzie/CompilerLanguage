#pragma once

#include "parser/ast.hpp"

#include <string>

namespace kite {

std::string type_name(const Type& type);
bool types_equal(const Type& left, const Type& right);

}  // namespace kite

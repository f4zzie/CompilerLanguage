#pragma once

#include "lexer/token.hpp"

#include <string>
#include <vector>

namespace kite {

std::string dump_tokens(const std::vector<Token>& tokens);

}  // namespace kite

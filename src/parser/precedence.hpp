#pragma once

namespace kite {

enum class Precedence : int {
  Assignment = 1,
  Pipe,
  Or,
  And,
  Equality,
  Comparison,
  Term,
  Factor,
  Unary,
  Postfix,
  Primary,
};

}  // namespace kite

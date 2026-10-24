# Fuzz tests

T025 adds bounded lexer/parser crash-resistance coverage here. The fuzz target must never crash or
hang on arbitrary bytes and must use a deterministic seed for ordinary CI.

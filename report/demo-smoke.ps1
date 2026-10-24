$ErrorActionPreference = "Stop"
cmake --preset linux-debug
cmake --build --preset linux-debug
& build/linux-debug/kite run examples/hello.kite
& build/linux-debug/kite run examples/hello-world.kite
& build/linux-debug/kite tokens examples/recursion.kite
& build/linux-debug/kite ast examples/recursion.kite
& build/linux-debug/kite disasm examples/recursion.kite
& build/linux-debug/kite run examples/pipe.kite --trace
ctest --test-dir build/linux-debug --output-on-failure

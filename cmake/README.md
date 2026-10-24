# CMake support

The root `CMakeLists.txt` owns the build. Presets select Debug, sanitizer, or Release builds.
Platform-specific warning and sanitizer flags are intentionally kept in the root file until the
project is large enough to justify separate modules.

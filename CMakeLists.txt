cmake_minimum_required(VERSION 3.20)

project(Kite LANGUAGES CXX VERSION 0.1.0)

option(KITE_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(KITE_BUILD_TESTS "Build Kite tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

if(KITE_ENABLE_SANITIZERS AND NOT MSVC)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

add_library(kite_lib
  src/diagnostics/diagnostic.cpp
  src/diagnostics/source_manager.cpp
  src/lexer/lexer.cpp
  src/lexer/token_dump.cpp
  src/parser/parser.cpp
  src/parser/ast_printer.cpp
  src/sema/sema.cpp
  src/sema/type.cpp
  src/ir/ir.cpp
  src/codegen/bytecode.cpp
  src/codegen/code_generator.cpp
  src/runtime/vm.cpp
  src/runtime/builtins.cpp
  src/driver/pipeline.cpp
)

target_include_directories(kite_lib PUBLIC src)

add_executable(kite src/driver/main.cpp)
target_link_libraries(kite PRIVATE kite_lib)

if(KITE_BUILD_TESTS)
  enable_testing()
  add_executable(kite_tests
    tests/test_main.cpp
    tests/unit/diagnostics_test.cpp
    tests/unit/lexer_test.cpp
  )
  target_include_directories(kite_tests PRIVATE src third_party)
  target_link_libraries(kite_tests PRIVATE kite_lib)
  add_test(NAME kite_unit_tests COMMAND kite_tests --test-case-exclude=walking_skeleton)
  add_test(NAME walking_skeleton COMMAND kite_tests --test-case=walking_skeleton)
endif()

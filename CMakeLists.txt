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
  src/sema/symbol.cpp
  src/sema/type.cpp
  src/ir/ir.cpp
  src/codegen/bytecode.cpp
  src/codegen/code_generator.cpp
  src/runtime/vm.cpp
  src/runtime/builtins.cpp
  src/driver/pipeline.cpp
  src/driver/cli.cpp
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
    tests/unit/ast_test.cpp
    tests/unit/parser_expression_test.cpp
    tests/unit/parser_statement_test.cpp
    tests/unit/symbol_test.cpp
    tests/unit/sema_test.cpp
    tests/unit/bytecode_test.cpp
    tests/unit/codegen_test.cpp
    tests/unit/vm_test.cpp
    tests/unit/driver_test.cpp
    tests/unit/variables_test.cpp
    tests/unit/control_flow_test.cpp
    tests/unit/functions_test.cpp
    tests/unit/recursion_test.cpp
    tests/unit/builtins_test.cpp
    tests/unit/pipe_test.cpp
    tests/unit/arrays_for_test.cpp
  )
  target_include_directories(kite_tests PRIVATE src third_party)
  target_link_libraries(kite_tests PRIVATE kite_lib)
  add_executable(kite_fuzz_smoke tests/fuzz/lexer_parser_fuzz.cpp)
  target_include_directories(kite_fuzz_smoke PRIVATE src)
  target_link_libraries(kite_fuzz_smoke PRIVATE kite_lib)
  add_test(NAME kite_unit_tests COMMAND kite_tests)
  add_test(NAME walking_skeleton
           COMMAND ${CMAKE_COMMAND}
                   -DKITE_EXECUTABLE=$<TARGET_FILE:kite>
                   -DKITE_SOURCE_FILE=${CMAKE_SOURCE_DIR}/examples/hello.kite
                   -P ${CMAKE_SOURCE_DIR}/tests/e2e/walking_skeleton_test.cmake)
  add_test(NAME fuzz_lexer_parser COMMAND kite_fuzz_smoke)
  add_test(NAME examples_corpus
           COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/check-examples.py
                   $<TARGET_FILE:kite> ${CMAKE_SOURCE_DIR}/examples)
  foreach(golden_command IN ITEMS tokens ast disasm)
    add_test(NAME golden_hello_${golden_command}
             COMMAND ${CMAKE_COMMAND}
                     -DKITE_EXECUTABLE=$<TARGET_FILE:kite>
                     -DKITE_COMMAND=${golden_command}
                     -DKITE_SOURCE=tests/golden/hello.kite
                     -DKITE_WORKING_DIRECTORY=${CMAKE_SOURCE_DIR}
                     -DKITE_EXPECTED=${CMAKE_SOURCE_DIR}/tests/golden/hello.${golden_command}
                     -P ${CMAKE_SOURCE_DIR}/tests/golden/golden_test.cmake)
  endforeach()
endif()

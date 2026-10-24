execute_process(
  COMMAND "${KITE_EXECUTABLE}" run "${KITE_SOURCE_FILE}"
  RESULT_VARIABLE run_result
  OUTPUT_VARIABLE run_output
  ERROR_VARIABLE run_error
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT run_result EQUAL 0)
  message(FATAL_ERROR "walking skeleton exited ${run_result}: ${run_error}")
endif()

if(NOT run_output STREQUAL "3")
  message(FATAL_ERROR "walking skeleton output was [${run_output}], expected [3]")
endif()

set(invalid_source "${CMAKE_CURRENT_BINARY_DIR}/walking_skeleton_invalid.kite")
file(WRITE "${invalid_source}" "print(1 + );\n")
execute_process(
  COMMAND "${KITE_EXECUTABLE}" check "${invalid_source}"
  RESULT_VARIABLE invalid_result
  OUTPUT_VARIABLE invalid_output
  ERROR_VARIABLE invalid_error
)
if(NOT invalid_result EQUAL 1)
  message(FATAL_ERROR "invalid source exited ${invalid_result}, expected 1: ${invalid_error}")
endif()

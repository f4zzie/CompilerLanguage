execute_process(
  COMMAND "${KITE_EXECUTABLE}" "${KITE_COMMAND}" "${KITE_SOURCE}"
  WORKING_DIRECTORY "${KITE_WORKING_DIRECTORY}"
  RESULT_VARIABLE result
  OUTPUT_VARIABLE actual
  ERROR_VARIABLE error
)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "${KITE_COMMAND} failed with ${result}: ${error}")
endif()
file(READ "${KITE_EXPECTED}" expected)
if(NOT actual STREQUAL expected)
  message(FATAL_ERROR "${KITE_COMMAND} golden mismatch for ${KITE_SOURCE}\n--- expected ---\n${expected}\n--- actual ---\n${actual}")
endif()

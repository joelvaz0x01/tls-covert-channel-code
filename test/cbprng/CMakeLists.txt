add_executable(cbprng_test test_cbprng.c)

target_link_libraries(cbprng_test PRIVATE
  cbprng_lib
  rand_lib
)
set_target_properties(cbprng_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${CMAKE_SOURCE_DIR}/bin/test
)

add_custom_command(
  TARGET cbprng_test POST_BUILD
  COMMAND ${CMAKE_SOURCE_DIR}/bin/test/cbprng_test
  COMMENT "Running CBPRNG permutation tests..."
)
add_test(
  NAME CBPRNGDuplicateFinder
  COMMAND ${CMAKE_SOURCE_DIR}/bin/test/cbprng_test
)

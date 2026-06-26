add_executable(fountain_test test_fountain.c)

target_link_libraries(fountain_test PRIVATE
  fountain_code_lib
  rand_lib
  generic_utils
)
set_target_properties(fountain_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${CMAKE_SOURCE_DIR}/bin/test
)

add_custom_command(
  TARGET fountain_test POST_BUILD
  COMMAND ${CMAKE_SOURCE_DIR}/bin/test/fountain_test
  COMMENT "Running fountain-code implementation tests..."
)
add_test(
  NAME FountainCodeTest
  COMMAND ${CMAKE_SOURCE_DIR}/bin/test/fountain_test
)

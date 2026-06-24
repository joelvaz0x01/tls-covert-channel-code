add_executable(encoder_dieharder
  encoder_generator.c
  ${CMAKE_SOURCE_DIR}/src/fountain_code/encode.c
  ${CMAKE_SOURCE_DIR}/src/fountain_code/utils.c
)

target_link_libraries(encoder_dieharder PRIVATE
  cbprng_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
  m
)

target_include_directories(encoder_dieharder PRIVATE
  ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(encoder_dieharder PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/study/encoder/dieharder_test
)

configure_file(
  run_dieharder_tests.sh
  ${CMAKE_SOURCE_DIR}/bin/study/encoder/dieharder_test/run_dieharder_tests.sh
  COPYONLY
)
configure_file(
  ../input.txt
  ${CMAKE_SOURCE_DIR}/bin/study/encoder/input.txt
  COPYONLY
)
file(CHMOD
  ${CMAKE_SOURCE_DIR}/bin/study/encoder/dieharder_test/run_dieharder_tests.sh
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

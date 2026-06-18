set(TEST_CASES
  "4 8" "4 12" "4 16" "4 20" "4 24" "4 28"
  "8 8" "8 12" "8 16"
)

foreach(CASE IN LISTS TEST_CASES)
  string(REPLACE " " ";" CASE_LIST "${CASE}")
  list(GET CASE_LIST 0 BITS)
  list(GET CASE_LIST 1 LAYERS)
  set(TARGET_NAME cbprng_dieharder_${BITS}_${LAYERS})

  add_executable(${TARGET_NAME}
    cbprng_generator.c
    ${CMAKE_SOURCE_DIR}/src/cbprng/cbprng.c
    ${CMAKE_SOURCE_DIR}/src/cbprng/p_box.c
    ${CMAKE_SOURCE_DIR}/src/cbprng/s_box.c
  )

  target_link_libraries(${TARGET_NAME} PRIVATE
    rand_lib
    uint128_lib
    generic_utils
    m
  )

  target_include_directories(
    ${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
  )

  target_compile_definitions(${TARGET_NAME} PRIVATE
    S_BOX_BITS=${BITS}
    N_LAYERS=${LAYERS}
  )

  set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/study/cbprng/dieharder_test
  )
endforeach()

file(
  COPY run_dieharder_tests.sh
  DESTINATION ${CMAKE_SOURCE_DIR}/bin/study/cbprng/dieharder_test
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

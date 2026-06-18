find_library(TESTU01_LIBRARY NAMES testu01)
find_path(TESTU01_INCLUDE_DIR NAMES unif01.h)

if(NOT TESTU01_LIBRARY OR NOT TESTU01_INCLUDE_DIR)
  message(STATUS "--------------------------------------------------------")
  message(STATUS " NOTE: TestU01 library not found.")
  message(STATUS " Skipping compilation for module: rand_testu01_bigcrush")
  message(STATUS "--------------------------------------------------------")
  return()
endif()

add_executable(rand64_bigcrush rand64_bigcrush.c)
target_include_directories(rand64_bigcrush PRIVATE
  ${TESTU01_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(rand64_bigcrush PRIVATE rand_lib ${TESTU01_LIBRARY})

add_executable(rand128_bigcrush rand128_bigcrush.c)
target_include_directories(rand128_bigcrush PRIVATE
  ${TESTU01_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(rand128_bigcrush PRIVATE rand_lib ${TESTU01_LIBRARY})

set_target_properties(rand64_bigcrush rand128_bigcrush PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/study/rand/testu01
)

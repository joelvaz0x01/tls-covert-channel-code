cmake_minimum_required(VERSION 3.24)

project(tls-client-random-fountain-code C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)  # executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)  # shared libs (.so / .dylib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # static libs (.a)

# interprocedural optimization (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
if(ipo_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Default to Release build if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Global compiler options and definitions for all targets
add_compile_options(
  # GCC/Clang flags
  $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-Wall>
  $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-Wextra>
  $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-Wpedantic>
  $<$<AND:$<CONFIG:Release>,$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>>:-O3>
  $<$<AND:$<CONFIG:Release>,$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>>:-march=native>
  # MSVC flags
  $<$<C_COMPILER_ID:MSVC>:/W4>
  $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/O2>
  $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/Ot>
  $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/Oi>
  $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/Gy>
)

add_compile_definitions(
  $<$<CONFIG:Debug>:DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)

add_subdirectory(src)
add_subdirectory(study)

# implementation tests
enable_testing()

add_executable(fountain_test test/fountain_code/test_fountain.c)
target_link_libraries(fountain_test PRIVATE
  fountain_code_lib cbprng_lib hash_lib rand_lib uint128_lib generic_utils m
)
set_target_properties(fountain_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)


add_executable(cbprng_test test/cbprng/test_cbprng.c)
target_link_libraries(cbprng_test PRIVATE
  fountain_code_lib cbprng_lib hash_lib rand_lib uint128_lib generic_utils
)
set_target_properties(cbprng_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# global target configuration
set(ALL_TARGETS
    # Executables
    decoder
    tls_encoder
    encode_decode_demo
    # Tests
    fountain_test
    cbprng_test
)

foreach(TGT ${ALL_TARGETS})
  get_target_property(TGT_TYPE ${TGT} TYPE)
  if(TGT_TYPE STREQUAL "INTERFACE_LIBRARY")
    set(SCOPE INTERFACE)
  else()
    set(SCOPE PRIVATE)
  endif()

  # Warnings and Optimizations
  target_compile_options(${TGT} ${SCOPE}
    # GCC/Clang flags
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-Wall>
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-Wextra>
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-Wpedantic>
    $<$<AND:$<CONFIG:Release>,$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>>:-O3>
    $<$<AND:$<CONFIG:Release>,$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>>:-march=native>
    # MSVC flags
    $<$<C_COMPILER_ID:MSVC>:/W4>
    $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/O2>
    $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/Ot>
    $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/Oi>
    $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/Gy>
  )

  target_compile_definitions(${TGT} ${SCOPE}
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
  )
endforeach()

# fountain_code module tests
add_custom_command(
  TARGET fountain_test POST_BUILD
  COMMAND ${CMAKE_BINARY_DIR}/tests/fountain_test
  COMMENT "Running fountain-code implementation tests..."
)
add_test(
  NAME FountainCodeTest
  COMMAND ${CMAKE_BINARY_DIR}/tests/fountain_test
)

# cbprng module tests
add_custom_command(
  TARGET cbprng_test POST_BUILD
  COMMAND ${CMAKE_BINARY_DIR}/tests/cbprng_test
  COMMENT "Running CBPRNG permutation tests..."
)
add_test(
  NAME CBPRNGDuplicateFinder
  COMMAND ${CMAKE_BINARY_DIR}/tests/cbprng_test
)

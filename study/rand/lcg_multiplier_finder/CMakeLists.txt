find_library(NTL_LIBRARY NAMES ntl)
find_path(NTL_INCLUDE_DIR NAMES NTL/ZZ.h)

include(CheckLanguage)
check_language(CUDA)

if(NOT NTL_LIBRARY OR NOT NTL_INCLUDE_DIR OR NOT CMAKE_CUDA_COMPILER)
  message(STATUS "--------------------------------------------------------")
  message(STATUS " NOTE: NTL, GMP libraries or CUDA compiler not found.")
  message(STATUS " Skipping compilation for module: lcg_multiplier_finder")
  message(STATUS "--------------------------------------------------------")
  return()
endif()

enable_language(CXX CUDA)

add_executable(lcg_multiplier_finder
  gpu_engine_64.cu
  gpu_engine_128.cu
  lcg_multiplier_finder.cu
  spectral_test.cpp
)

target_include_directories(lcg_multiplier_finder PRIVATE ${NTL_INCLUDE_DIR})
target_link_libraries(lcg_multiplier_finder PRIVATE ${NTL_LIBRARY})

set_target_properties(lcg_multiplier_finder PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  CUDA_STANDARD 17
  CUDA_STANDARD_REQUIRED ON
  CUDA_SEPARABLE_COMPILATION ON
  CUDA_ARCHITECTURES native
  INTERPROCEDURAL_OPTIMIZATION OFF
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(lcg_multiplier_finder PRIVATE -Wno-pedantic)
endif()

set_target_properties(lcg_multiplier_finder PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/study
)

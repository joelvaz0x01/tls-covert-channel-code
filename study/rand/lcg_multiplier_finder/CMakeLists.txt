find_library(NTL_LIBRARY NAMES ntl)
find_path(NTL_INCLUDE_DIR NAMES NTL/ZZ.h)

find_library(GMP_LIBRARY NAMES gmp mpir)
find_path(GMP_INCLUDE_DIR NAMES gmp.h mpir.h)

find_package(Threads REQUIRED)

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

find_package(CUDAToolkit REQUIRED)

add_executable(lcg_multiplier_finder
  gpu_engine_64.cu
  gpu_engine_128.cu
  lcg_multiplier_finder.cu
  spectral_test.cpp
)

target_include_directories(lcg_multiplier_finder PRIVATE
  ${NTL_INCLUDE_DIR}
  ${GMP_INCLUDE_DIR}
)

target_link_libraries(lcg_multiplier_finder PRIVATE
  ${NTL_LIBRARY}
  ${GMP_LIBRARY}
  CUDA::cudart_static
  Threads::Threads
)

if(UNIX AND NOT APPLE)
  target_link_options(lcg_multiplier_finder PRIVATE
    -static-libgcc
    -static-libstdc++
  )
endif()

set_target_properties(lcg_multiplier_finder PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  CUDA_STANDARD 23
  CUDA_STANDARD_REQUIRED ON
  CUDA_SEPARABLE_COMPILATION ON
  CUDA_ARCHITECTURES native
  INTERPROCEDURAL_OPTIMIZATION OFF
)

target_compile_options(lcg_multiplier_finder PRIVATE
  $<$<COMPILE_LANGUAGE:CUDA>:
    -Xptxas=-O3
    -ftz=true
    -fmad=true
  >
)

if(MSVC)
  target_compile_options(lcg_multiplier_finder PRIVATE /bigobj)
  target_compile_definitions(lcg_multiplier_finder PRIVATE NOMINMAX)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(lcg_multiplier_finder PRIVATE -Wno-pedantic)
endif()

set_target_properties(lcg_multiplier_finder PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/study/rand/lcg_multiplier_finder
)

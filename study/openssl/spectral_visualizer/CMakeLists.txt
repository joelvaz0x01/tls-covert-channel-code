find_package(OpenSSL QUIET)

if(NOT OpenSSL_FOUND)
  message(STATUS "--------------------------------------------------------------")
  message(STATUS " NOTE: OpenSSL library not found.")
  message(STATUS " Skipping compilation for module: openssl_spectral_visualizer")
  message(STATUS "--------------------------------------------------------------")
  return()
endif()

add_subdirectory(openssl_rand_mod)

add_executable(openssl_spectral_visualizer
    openssl_spectral_visualizer.c
)
target_link_libraries(openssl_spectral_visualizer PRIVATE
    OpenSSL::Crypto
    openssl_rand_mod_lib
    spectral_visualizer
    rand_lib
    m
)
target_include_directories(openssl_spectral_visualizer PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

set(SPECTRAL_OUT_DIR ${CMAKE_SOURCE_DIR}/bin/study/openssl/spectral_visualizer)
set_target_properties(openssl_spectral_visualizer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${SPECTRAL_OUT_DIR}
)

add_custom_command(
  TARGET openssl_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/openssl_spectral_visualizer 50000
  COMMAND octave --no-gui openssl_spectral_plot.m
  COMMAND ${CMAKE_COMMAND} -E copy
    openssl_spectral.png
    ${CMAKE_SOURCE_DIR}/study_results/spectral_visualizer/openssl/openssl_spectral.png
  COMMAND ${CMAKE_COMMAND} -E remove -f
    openssl_spectral_data_2d.dat
    openssl_spectral_data_3d.dat
    openssl_spectral_plot.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating openssl_spectral.png..."
)

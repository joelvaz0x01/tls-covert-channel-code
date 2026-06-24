find_package(OpenSSL QUIET)

if(NOT OpenSSL_FOUND)
  message(STATUS "--------------------------------------------------------------")
  message(STATUS " NOTE: OpenSSL library not found.")
  message(STATUS " Skipping compilation for module: openssl_spectral_visualizer")
  message(STATUS "--------------------------------------------------------------")
  return()
endif()

add_executable(openssl_spectral_visualizer openssl_spectral_visualizer.c)
target_link_libraries(openssl_spectral_visualizer PRIVATE OpenSSL::Crypto spectral_visualizer m)

set(SPECTRAL_OUT_DIR ${CMAKE_SOURCE_DIR}/bin/study/openssl/spectral_visualizer)
set_target_properties(openssl_spectral_visualizer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${SPECTRAL_OUT_DIR}
)

add_custom_command(
  TARGET openssl_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/openssl_spectral_visualizer 2d 1000
  COMMAND octave --no-gui openssl_spectral_plot_2d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    openssl_spectral_data_2d.dat
    openssl_spectral_plot_2d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating openssl_spectral_2d.pdf..."
)

add_custom_command(
  TARGET openssl_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/openssl_spectral_visualizer 3d 1000
  COMMAND octave --no-gui openssl_spectral_plot_3d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    openssl_spectral_data_3d.dat
    openssl_spectral_plot_3d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating openssl_spectral_3d.pdf..."
)

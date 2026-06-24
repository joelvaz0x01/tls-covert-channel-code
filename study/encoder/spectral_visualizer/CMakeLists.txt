add_executable(encoder_spectral_visualizer encoder_spectral_visualizer.c)

target_link_libraries(encoder_spectral_visualizer PRIVATE
  spectral_visualizer
  cbprng_lib
  fountain_code_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
  m
)

target_include_directories(encoder_spectral_visualizer PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(SPECTRAL_OUT_DIR ${CMAKE_SOURCE_DIR}/bin/study/encoder/spectral_visualizer)

set_target_properties(encoder_spectral_visualizer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${SPECTRAL_OUT_DIR}
)

configure_file(
  ../input.txt
  ${SPECTRAL_OUT_DIR}/input.txt
  COPYONLY
)

add_custom_command(
  TARGET encoder_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/encoder_spectral_visualizer
    ${SPECTRAL_OUT_DIR}/input.txt 2d 1000
  COMMAND octave --no-gui encoder_spectral_plot_2d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    encoder_spectral_data_2d.dat
    encoder_spectral_plot_2d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating encoder_spectral_2d.pdf..."
)

add_custom_command(
  TARGET encoder_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/encoder_spectral_visualizer
    ${SPECTRAL_OUT_DIR}/input.txt 3d 1000
  COMMAND octave --no-gui encoder_spectral_plot_3d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    encoder_spectral_data_3d.dat
    encoder_spectral_plot_3d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating encoder_spectral_3d.pdf..."
)

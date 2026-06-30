add_executable(encoder_spectral_visualizer encoder_spectral_visualizer.c)

target_compile_definitions(encoder_spectral_visualizer PRIVATE MAX_BLOCKS=524288)

target_link_libraries(encoder_spectral_visualizer PRIVATE
  spectral_visualizer
  fountain_code_lib
  cbprng_lib
  hash_lib
  rand_lib
  generic_utils
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
    ${SPECTRAL_OUT_DIR}/input.txt 50000
  COMMAND octave --no-gui encoder_spectral_plot.m
  COMMAND ${CMAKE_COMMAND} -E copy
    encoder_spectral.png
    ${CMAKE_SOURCE_DIR}/study_results/spectral_visualizer/encoder/encoder_spectral.png
  COMMAND ${CMAKE_COMMAND} -E remove -f
    encoder_spectral_data_2d.dat
    encoder_spectral_data_3d.dat
    encoder_spectral_plot.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating encoder_spectral.png..."
)

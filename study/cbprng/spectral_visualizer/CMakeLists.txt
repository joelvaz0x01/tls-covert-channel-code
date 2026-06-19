add_executable(cbprng_spectral_visualizer cbprng_spectral_visualizer.c)

target_link_libraries(cbprng_spectral_visualizer PRIVATE cbprng_lib rand_lib m)

target_include_directories(cbprng_spectral_visualizer PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(SPECTRAL_OUT_DIR ${CMAKE_SOURCE_DIR}/bin/study/cbprng/spectral_visualizer)

set_target_properties(cbprng_spectral_visualizer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${SPECTRAL_OUT_DIR}
)

add_custom_command(
  TARGET cbprng_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/cbprng_spectral_visualizer 2d 1000
  COMMAND octave --no-gui cbprng_spectral_plot_2d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    cbprng_spectral_data_2d.dat
    cbprng_spectral_plot_2d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating cbprng_spectral_2d.pdf..."
)

add_custom_command(
  TARGET cbprng_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/cbprng_spectral_visualizer 3d 1000
  COMMAND octave --no-gui cbprng_spectral_plot_3d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    cbprng_spectral_data_3d.dat
    cbprng_spectral_plot_3d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating cbprng_spectral_3d.pdf..."
)

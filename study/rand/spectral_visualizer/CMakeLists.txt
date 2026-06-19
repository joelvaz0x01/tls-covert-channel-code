add_executable(rand64_spectral_visualizer  rand64_spectral_visualizer.c)
add_executable(rand128_spectral_visualizer rand128_spectral_visualizer.c)

target_link_libraries(rand64_spectral_visualizer  PRIVATE rand_lib m)
target_link_libraries(rand128_spectral_visualizer PRIVATE rand_lib m)

target_include_directories(rand64_spectral_visualizer  PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(rand128_spectral_visualizer PRIVATE ${CMAKE_SOURCE_DIR}/src)

set(SPECTRAL_OUT_DIR ${CMAKE_SOURCE_DIR}/bin/study/rand/spectral_visualizer)

set_target_properties(rand64_spectral_visualizer rand128_spectral_visualizer PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${SPECTRAL_OUT_DIR}
)

add_custom_command(
  TARGET rand64_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/rand64_spectral_visualizer 2d 1000
  COMMAND octave --no-gui rand64_spectral_plot_2d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    rand64_spectral_data_2d.dat
    rand64_spectral_plot_2d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating rand64_spectral_2d.pdf..."
)

add_custom_command(
  TARGET rand64_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/rand64_spectral_visualizer 3d 1000
  COMMAND octave --no-gui rand64_spectral_plot_3d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    rand64_spectral_data_3d.dat
    rand64_spectral_plot_3d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating rand64_spectral_3d.pdf..."
)

add_custom_command(
  TARGET rand128_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/rand128_spectral_visualizer 2d 1000
  COMMAND octave --no-gui rand128_spectral_plot_2d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    rand128_spectral_data_2d.dat
    rand128_spectral_plot_2d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating rand128_spectral_2d.pdf..."
)

add_custom_command(
  TARGET rand128_spectral_visualizer POST_BUILD
  COMMAND ${SPECTRAL_OUT_DIR}/rand128_spectral_visualizer 3d 1000
  COMMAND octave --no-gui rand128_spectral_plot_3d.m
  COMMAND ${CMAKE_COMMAND} -E remove -f
    rand128_spectral_data_3d.dat
    rand128_spectral_plot_3d.m
  WORKING_DIRECTORY ${SPECTRAL_OUT_DIR}
  COMMENT "Generating rand128_spectral_3d.pdf..."
)

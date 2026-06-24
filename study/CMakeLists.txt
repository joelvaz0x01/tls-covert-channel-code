add_subdirectory(cbprng)
add_subdirectory(encoder)
add_subdirectory(openssl)
add_subdirectory(rand)

add_library(spectral_visualizer STATIC spectral_visualizer.c)
target_include_directories(spectral_visualizer PUBLIC ${CMAKE_SOURCE_DIR})

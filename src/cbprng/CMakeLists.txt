add_library(cbprng_lib INTERFACE)

target_sources(cbprng_lib INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/cbprng.c
  ${CMAKE_CURRENT_LIST_DIR}/s_box.c
)

target_include_directories(cbprng_lib INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

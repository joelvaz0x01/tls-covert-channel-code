add_library(fountain_code_lib INTERFACE)

target_sources(fountain_code_lib INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/encode.c
  ${CMAKE_CURRENT_LIST_DIR}/decode.c
  ${CMAKE_CURRENT_LIST_DIR}/utils.c
)

target_include_directories(fountain_code_lib INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(fountain_code_lib INTERFACE m)

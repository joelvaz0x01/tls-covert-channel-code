add_library(fountain_code_lib STATIC
  encode.c
  decoder.c
  utils.c
)

target_include_directories(fountain_code_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

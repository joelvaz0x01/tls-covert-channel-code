add_library(fountain_code_lib OBJECT
  encode.c
  decode.c
  utils.c
)

target_include_directories(fountain_code_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

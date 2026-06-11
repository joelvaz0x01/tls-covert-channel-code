add_library(cbprng_lib STATIC
  cbprng.c
  p_box.c
  s_box.c
)

target_include_directories(cbprng_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

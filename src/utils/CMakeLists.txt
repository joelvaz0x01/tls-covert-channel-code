add_library(generic_utils OBJECT
  decoder.c
  encoder.c
  file.c
  print.c
  utils.c
)

target_include_directories(generic_utils PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

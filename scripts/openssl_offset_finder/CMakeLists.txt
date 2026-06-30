add_executable(openssl_ret_offset
  dwarf.c
  elf.c
  openssl_detect.c
  openssl_offset_finder.c
)
target_compile_definitions(openssl_ret_offset PRIVATE RET_LINE=1)
target_link_libraries(openssl_ret_offset PRIVATE ${CMAKE_DL_LIBS})

set_target_properties(openssl_ret_offset PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/apache_patch_hello
)

add_executable(openssl_2th_rand_offset
  dwarf.c
  elf.c
  openssl_detect.c
  openssl_offset_finder.c
  x86_scan.c
)
target_link_libraries(openssl_2th_rand_offset PRIVATE ${CMAKE_DL_LIBS})

set_target_properties(openssl_2th_rand_offset PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/scripts
)

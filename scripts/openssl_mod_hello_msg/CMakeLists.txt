find_package(OpenSSL REQUIRED)

add_library(FOUNTAIN_bytes_ex_obj OBJECT FOUNTAIN_bytes_ex.c)
target_compile_options(FOUNTAIN_bytes_ex_obj PRIVATE -march=x86-64 -fno-lto)

add_executable(FOUNTAIN_bytes_ex_test
  $<TARGET_OBJECTS:FOUNTAIN_bytes_ex_obj>
  test_FOUNTAIN_bytes_ex.c
)

target_link_libraries(FOUNTAIN_bytes_ex_test PRIVATE OpenSSL::Crypto)

set_target_properties(FOUNTAIN_bytes_ex_test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/scripts
)

set(CODE_BIN ${CMAKE_SOURCE_DIR}/bin/scripts/code.bin)

add_custom_command(
  OUTPUT ${CODE_BIN}
  COMMAND ${CMAKE_OBJCOPY} -O binary -j .text
    $<TARGET_OBJECTS:FOUNTAIN_bytes_ex_obj>
    ${CODE_BIN}
  DEPENDS FOUNTAIN_bytes_ex_obj
  COMMENT "Extracting raw .text machine code to code.bin"
)

add_custom_target(generate_code_bin ALL
  DEPENDS ${CODE_BIN}
)

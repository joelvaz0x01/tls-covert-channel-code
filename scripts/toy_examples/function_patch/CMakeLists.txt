add_library(function_patch_toy_example_obj OBJECT
  modified_function.c

)
target_compile_options(function_patch_toy_example_obj PRIVATE
  -march=x86-64
  -fno-lto
)

add_executable(victim_program
  $<TARGET_OBJECTS:function_patch_toy_example_obj>
  victim_program.c
)

set_target_properties(victim_program PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/function_patch
)

set(CODE_BIN ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/function_patch/code.bin)

add_custom_command(
  OUTPUT ${CODE_BIN}
  COMMAND ${CMAKE_OBJCOPY} -O binary -j .text
    $<TARGET_OBJECTS:function_patch_toy_example_obj>
    ${CODE_BIN}
  DEPENDS function_patch_toy_example_obj
  COMMENT "Extracting raw .text machine code to code.bin"
)

add_custom_target(generate_toy_example_code_bin ALL
  DEPENDS ${CODE_BIN}
)

configure_file(
  run_patch.bash
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/function_patch/run_patch.bash
  COPYONLY
)

file(CHMOD
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/function_patch/run_patch.bash
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

configure_file(
  ../data.bin
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/function_patch/data.bin
  COPYONLY
)

file(CHMOD
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/function_patch/data.bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

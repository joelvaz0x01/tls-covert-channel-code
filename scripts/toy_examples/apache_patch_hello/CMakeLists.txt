configure_file(
  run_gdb.sh
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/apache_patch_hello/run_gdb.sh
  COPYONLY
)

file(CHMOD
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/apache_patch_hello/run_gdb.sh
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

configure_file(
  ../data.bin
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/apache_patch_hello/data.bin
  COPYONLY
)

file(CHMOD
  ${CMAKE_SOURCE_DIR}/bin/scripts/toy_examples/apache_patch_hello/data.bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

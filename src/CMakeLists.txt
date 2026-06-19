add_subdirectory(cbprng)
add_subdirectory(fountain_code)
add_subdirectory(hash)
add_subdirectory(rand)
add_subdirectory(uint128)
add_subdirectory(utils)

# main library aggregation
add_library(modified_tls_random INTERFACE)
target_link_libraries(modified_tls_random INTERFACE
  fountain_code_lib
  cbprng_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
)

# executables
add_executable(encoder tls_encoder.c)
target_link_libraries(encoder PRIVATE modified_tls_random m)

add_executable(decoder tls_decoder.c)
target_link_libraries(decoder PRIVATE modified_tls_random m)

add_executable(fountain_code_demo tls_fc_demo.c)
target_link_libraries(fountain_code_demo PRIVATE modified_tls_random m)

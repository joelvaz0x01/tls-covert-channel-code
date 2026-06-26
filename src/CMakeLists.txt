add_subdirectory(cbprng)
add_subdirectory(fountain_code)
add_subdirectory(hash)
add_subdirectory(rand)
add_subdirectory(uint128)
add_subdirectory(utils)

# executables
add_executable(tls_encoder tls_encoder.c)
target_link_libraries(tls_encoder PRIVATE
  fountain_code_lib
  cbprng_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
)

add_executable(tls_decoder tls_decoder.c)
target_link_libraries(tls_decoder PRIVATE
  fountain_code_lib
  cbprng_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
)

add_executable(demo demo.c)
target_link_libraries(demo PRIVATE
  fountain_code_lib
  rand_lib
  generic_utils
)

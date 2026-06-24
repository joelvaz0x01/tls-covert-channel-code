add_subdirectory(cbprng)
add_subdirectory(fountain_code)
add_subdirectory(hash)
add_subdirectory(rand)
add_subdirectory(uint128)
add_subdirectory(utils)

# executables
add_executable(encoder encoder.c)
target_link_libraries(encoder PRIVATE
  fountain_code_lib
  cbprng_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
  m
)

add_executable(decoder decoder.c)
target_link_libraries(decoder PRIVATE
  fountain_code_lib
  cbprng_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
  m
)

add_executable(encode_decode_demo demo.c)
target_link_libraries(encode_decode_demo PRIVATE
  fountain_code_lib
  cbprng_lib
  hash_lib
  rand_lib
  uint128_lib
  generic_utils
  m
)

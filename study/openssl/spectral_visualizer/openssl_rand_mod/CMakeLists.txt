add_library(openssl_rand_mod_lib OBJECT
  det_src.c
  seed_setup.c
)

target_include_directories(openssl_rand_mod_lib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)

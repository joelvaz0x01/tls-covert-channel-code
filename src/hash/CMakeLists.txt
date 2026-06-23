add_library(hash_lib OBJECT hash.c sha256.c)

target_include_directories(hash_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

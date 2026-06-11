add_library(generic_utils STATIC utils.c)

target_include_directories(generic_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

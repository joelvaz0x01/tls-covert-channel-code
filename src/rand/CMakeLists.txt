add_library(rand_lib OBJECT
  rand64.c
  rand128.c
  system.c
  time.c
)

target_include_directories(rand_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

if(WIN32)
  target_link_libraries(rand_lib PUBLIC bcrypt)
endif()

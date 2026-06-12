add_library(rand64_lib STATIC rand64.c state_snapshot.c system.c time.c)

target_include_directories(rand64_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(WIN32)
  target_link_libraries(rand64_lib PUBLIC bcrypt)
endif()

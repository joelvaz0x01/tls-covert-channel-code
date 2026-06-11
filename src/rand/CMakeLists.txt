add_library(random_utils STATIC rand.c system.c time.c)

target_include_directories(random_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(WIN32)
  target_link_libraries(random_utils PUBLIC bcrypt)
endif()

cmake_minimum_required(VERSION 3.10)

project(tls-client-random-fountain-code C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)  # executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)  # shared libs (.so / .dylib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # static libs (.a)


# shared logic used by all executables
set(CORE_SOURCES
    src/fountain-code/fountain_code.c
    src/cbprng/cbprng.c
    src/hash/hash.c
    src/utils/print_byte_arrays.c
)

add_library(fountain_code_lib STATIC ${CORE_SOURCES})

target_include_directories(fountain_code_lib PUBLIC include)
target_link_libraries(fountain_code_lib PUBLIC m)


# executables
add_executable(encoder src/encoder.c)
target_link_libraries(encoder PRIVATE fountain_code_lib)

add_executable(decoder src/decoder.c)
target_link_libraries(decoder PRIVATE fountain_code_lib)

add_executable(fountain_code_demo src/fountain_code_demo.c)
target_link_libraries(fountain_code_demo PRIVATE fountain_code_lib)


# global target configuration
set(ALL_TARGETS
    encoder
    decoder
    fountain_code_demo
    fountain_code_lib
)

foreach(TGT ${ALL_TARGETS})
  target_compile_options(${TGT} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

  target_compile_definitions(${TGT} PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )
endforeach()


# implementation tests
enable_testing()
add_executable(fountain_test test/fountain_code/test_fountain.c)
set_target_properties(fountain_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
target_link_libraries(fountain_test PRIVATE fountain_code_lib)
target_compile_options(fountain_test PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(fountain_test PRIVATE $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Release>:NDEBUG>)

add_custom_command(TARGET fountain_test POST_BUILD
    COMMAND ${CMAKE_BINARY_DIR}/tests/fountain_test
    COMMENT "Running implementation tests..."
)

add_test(NAME FountainCodeTest COMMAND ${CMAKE_BINARY_DIR}/tests/fountain_test)

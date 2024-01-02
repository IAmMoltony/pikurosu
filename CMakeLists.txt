cmake_minimum_required(VERSION 3.0)

project(Pikurosu)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRECTORIES})

file(GLOB SRC_FILES src/*.c lib/libmtnlog/source/*.c)
include_directories(inc)
include_directories(lib/libmtnlog/include)
add_executable(Pikurosu ${SRC_FILES})

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Pikurosu PRIVATE -Wall -Wextra -g)
elseif (MSVC)
    target_compile_options(Pikurosu PRIVATE /W4)
endif ()

target_link_libraries(Pikurosu ${SDL2_LIBRARIES})

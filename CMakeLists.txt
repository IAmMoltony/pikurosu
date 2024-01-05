cmake_minimum_required(VERSION 3.0)

project(Pikurosu)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Threads REQUIRED)

file(GLOB SRC_FILES src/*.c lib/libmtnlog/source/*.c)
include_directories(inc)
include_directories(lib/libmtnlog/include)
add_executable(Pikurosu ${SRC_FILES})

target_compile_options(Pikurosu PRIVATE -Wall -Wextra -g)
target_link_options(Pikurosu PRIVATE -lSDL2_ttf)

target_link_libraries(Pikurosu ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} Threads::Threads)

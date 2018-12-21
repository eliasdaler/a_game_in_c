cmake_minimum_required(VERSION 3.13)

project(a_game_in_c
  LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

add_executable(game)

set_property(TARGET game PROPERTY C_STANDARD 99)
target_compile_options(game PRIVATE -Wall -pedantic)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

target_include_directories(game
  PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(game
  PRIVATE
    m
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
)

target_sources(game
  PUBLIC
    src/main.c
)

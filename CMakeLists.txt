cmake_minimum_required(VERSION 3.8)

project(a_game_in_c
  LANGUAGES C
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

add_executable(game
    src/main.c
    src/sdl_util.c
    src/sprite.c
    src/tile_map.c
)

set_property(TARGET game PROPERTY C_STANDARD 99)
set_property(TARGET game PROPERTY C_EXTENSIONS OFF)
target_compile_options(game PRIVATE -Wall -Wextra -Wformat -Wno-unused-parameter -Wpedantic)

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

# Copy resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# install rules
set(GAME_INSTALL_FOLDER "a_game_in_c")
install(TARGETS game
  RUNTIME DESTINATION ${GAME_INSTALL_FOLDER})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res
  DESTINATION ${GAME_INSTALL_FOLDER})

cmake_minimum_required(VERSION 3.8)

project(a_game_in_c
  LANGUAGES C
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

add_executable(game
    src/entity.c
    src/entity_factory.c
    src/entity_manager.c
    src/hash_table.c
    src/main.c
    src/resource_manager.c
    src/sdl_util.c
    src/sprite.c
    src/string_util.c
    src/tile_map.c

    # tests
    src/hash_table_test.c
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

set(GAME_RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)

# Add symlink to res folder to not copy it all the time during build
add_custom_command(TARGET game POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${GAME_RES_DIR}" "$<TARGET_FILE_DIR:game>/res"
)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# install rules
set(GAME_INSTALL_FOLDER "a_game_in_c")
install(TARGETS game
  RUNTIME DESTINATION ${GAME_INSTALL_FOLDER})

install(DIRECTORY ${GAME_RES_DIR}
  DESTINATION ${GAME_INSTALL_FOLDER})

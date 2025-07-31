add_executable(scrabble main.cpp)

add_subdirectory(core)
add_subdirectory(game)
add_subdirectory(cli)
add_subdirectory(gui)

target_link_libraries(scrabble PRIVATE
  core
  game
  cli
  gui
)

target_include_directories(scrabble PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(gui PRIVATE SDL_MAIN_HANDLED)

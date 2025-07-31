add_library(gui STATIC
  main_menu.cpp
  settings.cpp
  select_num_players.cpp
  gui.cpp
)

target_include_directories(gui PUBLIC ${CMAKE_SOURCE_DIR}/scrabble)
target_link_libraries(gui PUBLIC
  core
  external
  SDL2::SDL2
  SDL2_ttf::SDL2_ttf
  SDL2_image::SDL2_image
)

add_library(gui STATIC
  main_menu_state.cpp
  settings_state.cpp
  select_num_players_state.cpp
  input_names_state.cpp
  ingame_state.cpp
  resource_manager.cpp
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

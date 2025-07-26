add_library(cli STATIC
    game_cli.cpp
)

target_include_directories(cli PRIVATE ${CMAKE_SOURCE_DIR}/scrabble)
target_link_libraries(cli PRIVATE core game)

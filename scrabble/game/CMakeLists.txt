add_library(game STATIC
    game.cpp
)

target_include_directories(game PRIVATE ${CMAKE_SOURCE_DIR}/scrabble)
target_link_libraries(game PRIVATE core)

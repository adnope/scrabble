add_library(core STATIC
    bag.cpp
    board.cpp
    dictionary.cpp
    player.cpp
    square.cpp
    tile.cpp
    lexicon.cpp
    bot.cpp
)

target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/scrabble)
target_link_libraries(core PUBLIC external)

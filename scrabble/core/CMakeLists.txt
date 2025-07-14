add_library(core STATIC
  dictionary.cpp
  tile.cpp
  bag.cpp
  board.cpp
  square.cpp
  player.cpp
)

target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/scrabble)
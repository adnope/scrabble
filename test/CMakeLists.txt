add_executable(scrabble_test scrabble_test.cpp)

target_link_libraries(scrabble_test PRIVATE
  core
  external
  game
)
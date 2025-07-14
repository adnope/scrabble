add_executable(scrabble_test scrabble_test.cpp)

target_link_libraries(scrabble_test PRIVATE
  core
  external
)

enable_testing()
include(CTest)
add_test(NAME scrabble_test COMMAND scrabble_test)
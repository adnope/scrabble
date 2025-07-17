add_executable(scrabble_test scrabble_test.cpp)

target_link_libraries(scrabble_test PRIVATE
  core
  external
)

target_include_directories(scrabble_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/new-logic)
add_subdirectory(new-logic)
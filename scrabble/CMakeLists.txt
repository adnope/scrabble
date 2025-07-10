add_executable(scrabble main.cpp)

add_subdirectory(core)

target_include_directories(scrabble PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/header
)
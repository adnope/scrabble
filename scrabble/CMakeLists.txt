add_executable(scrabble main.cpp)

target_link_libraries(scrabble
    external
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

target_include_directories(scrabble PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
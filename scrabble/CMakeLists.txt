add_executable(scrabble main.cpp)

target_link_libraries(scrabble PRIVATE
    external
    core
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

target_include_directories(scrabble PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(core)
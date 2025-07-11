add_executable(scrabble main.cpp)

add_subdirectory(core)

target_link_libraries(scrabble PRIVATE
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
)

target_include_directories(scrabble PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/header
)

target_compile_definitions(scrabble PRIVATE SDL_MAIN_HANDLED)
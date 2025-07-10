cmake_minimum_required(VERSION 3.14)
project(c4ai LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)

# include(FetchContent)

# FetchContent_Declare(
#   SDL2
#   GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#   GIT_TAG release-2.32.8
# )
# FetchContent_MakeAvailable(SDL2)

# FetchContent_Declare(
#   SDL2_image
#   GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
#   GIT_TAG release-2.8.8
# )
# FetchContent_MakeAvailable(SDL2_image)

# FetchContent_Declare(
#   SDL2_ttf
#   GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
#   GIT_TAG release-2.24.0
# )
# FetchContent_MakeAvailable(SDL2_ttf)

# find_package(SDL2 CONFIG REQUIRED)
# message(STATUS "SDL2_FOUND = ${SDL2_FOUND}")
# message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
# message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")
# message(STATUS "SDL2_VERSION = ${SDL2_VERSION}")

add_subdirectory(scrabble)
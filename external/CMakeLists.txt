add_library(external INTERFACE)

target_include_directories(external INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

project(spaced-lib VERSION 0.1.0)

set(generated_header_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file(build_version.hpp.in "${generated_header_dir}/spaced/build_version.hpp" @ONLY)

add_library(${PROJECT_NAME})
add_library(spaced::lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
  bave::bgf
  bave::bave-compile-options
)

target_include_directories(${PROJECT_NAME} PUBLIC
  .
  "${generated_header_dir}"
)

file(GLOB_RECURSE sources LIST_DIRECTORIES false CONFIGURE_DEPENDS "spaced/*.?pp")

target_sources(${PROJECT_NAME} PRIVATE ${sources})

target_precompile_headers(${PROJECT_NAME} REUSE_FROM bave)

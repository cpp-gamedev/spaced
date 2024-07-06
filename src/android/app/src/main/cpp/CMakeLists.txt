cmake_minimum_required(VERSION 3.22)

project(spaced-android)

set(spaced_path ../../../../../../)
file(REAL_PATH ${spaced_path} spaced_path)
add_subdirectory(${spaced_path} "${CMAKE_CURRENT_BINARY_DIR}/spaced")

add_library(${PROJECT_NAME} SHARED)

target_link_libraries(${PROJECT_NAME} PRIVATE
  spaced::lib
)

target_sources(${PROJECT_NAME} PRIVATE
  main.cpp
)

add_custom_target(copy-assets ALL
  ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${spaced_path}/assets" "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_CURRENT_SOURCE_DIR}/../assets/assets.zip"
)

add_dependencies(${PROJECT_NAME} copy-assets)

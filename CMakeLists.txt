cmake_minimum_required(VERSION 3.22)

project(spaced)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  bave
  GIT_REPOSITORY https://github.com/karnkaul/bave
  GIT_TAG v0.5.4
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/bave"
)

FetchContent_MakeAvailable(bave)

add_subdirectory(src)

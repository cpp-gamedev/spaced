cmake_minimum_required(VERSION 3.22)

project(spaced)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  bgf
  GIT_REPOSITORY https://github.com/karnkaul/bgf
  GIT_TAG v0.1.2
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/bgf"
)

FetchContent_MakeAvailable(bgf)

add_subdirectory(src)

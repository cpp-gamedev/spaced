add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE spaced::lib)

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

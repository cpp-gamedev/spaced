add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE spaced::lib)

target_sources(${PROJECT_NAME} PRIVATE main.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME NovaSwarm)

if(WIN32)
  target_sources(${PROJECT_NAME} PRIVATE windows/app.rc)
  if(MSVC)
    set(subsystem $<IF:$<BOOL:$<CONFIG:Debug>>,CONSOLE,WINDOWS>)
    target_link_options(${PROJECT_NAME} PRIVATE
      /SUBSYSTEM:${subsystem}
      /ENTRY:mainCRTStartup
    )
  endif()
endif()

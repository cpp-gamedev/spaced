cmake_minimum_required(VERSION 3.24)

if("${PACKAGE_NAME}" STREQUAL "")
  message(WARNING "'PACKAGE_NAME' unset, using NovaSwarm.zip")
  set(PACKAGE_NAME "NovaSwarm.zip")
endif()

message(STATUS "package name: ${PACKAGE_NAME}")

set(exe_name "NovaSwarm")
set(assets_zip assets.zip)
set(exe_ext "")

if(WIN32)
  set(exe_ext .exe)
endif()

set(exe_filename ${exe_name}${exe_ext})
set(build_dir out/release)

message(STATUS "building to: ${build_dir}")

message(STATUS "configuring")

if(WIN32)
  set(preset vs22)
else()
  set(preset default)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -S . -B ${build_dir} --preset=${preset}
  COMMAND_ERROR_IS_FATAL ANY
)

message(STATUS "building game")
execute_process(COMMAND ${CMAKE_COMMAND} --build ${build_dir} --config=Release
  COMMAND_ERROR_IS_FATAL ANY
)

file(COPY ${build_dir}/src/desktop/Release/${exe_filename} DESTINATION ${build_dir})

message(STATUS "zipping assets")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar -cvf ../${build_dir}/${assets_zip} --format=zip
  assets

  WORKING_DIRECTORY assets
  COMMAND_ERROR_IS_FATAL ANY
)

message(STATUS "creating archive")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar -cvf ${PACKAGE_NAME} --format=zip
  ${exe_filename}
  ${assets_zip}

  WORKING_DIRECTORY ${build_dir}
  COMMAND_ERROR_IS_FATAL ANY
)

file(COPY ${build_dir}/${PACKAGE_NAME} DESTINATION .)

message(STATUS "release built and packaged to '${PACKAGE_NAME}'")

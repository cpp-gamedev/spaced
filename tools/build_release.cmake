cmake_minimum_required(VERSION 3.24)

set(build_version "")
set(exe_name "NovaSwarm")
set(exe_ext "")

if(WIN32)
  set(exe_ext .exe)
endif()

set(exe_filename ${exe_name}${exe_ext})

if("${build_version}" STREQUAL "")
  file(READ build_version.txt build_version)
endif()

if("${build_version}" STREQUAL "")
  set(build_version "unknown")
  message(WARNING "failed to read build version, forcing to '${build_version}'")
endif()

set(build_dir out/release-${build_version})

message(STATUS "building to: ${build_dir}")

message(STATUS "configuring")
execute_process(COMMAND ${CMAKE_COMMAND} -G "Ninja Multi-Config" -S . -B ${build_dir})

message(STATUS "building game")
execute_process(COMMAND ${CMAKE_COMMAND} --build ${build_dir} --config=Release)

file(COPY ${build_dir}/src/desktop/Release/spaced${exe_ext} DESTINATION ${build_dir}/package)

file(RENAME "${build_dir}/package/spaced" "${build_dir}/package/${exe_name}")

set(assets_zip_path assets/assets.zip)
set(assets_to_zip "")

if(EXISTS ${assets_zip_path})
  message("using '${assets_zip_path}'")
  file(COPY ${assets_zip_path} DESTINATION ${build_dir}/package)
  set(assets_to_zip assets.zip)
else()
  message(WARNING "'${assets_zip_path}' not found, using loose files")
  file(COPY assets DESTINATION ${build_dir}/package)
  set(assets_to_zip assets)
endif()

message(STATUS "creating archive")
set(package_name ${exe_name}-v${build_version}.zip)
execute_process(COMMAND ${CMAKE_COMMAND} -E tar -cvf ${package_name} --format=zip
  ${exe_filename}
  ${assets_to_zip}

  WORKING_DIRECTORY ${build_dir}/package
)

message(STATUS "release built and packaged to '${build_dir}/package/${package_name}'")

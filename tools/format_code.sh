#!/bin/bash 

[[ ! $(clang-format --version) ]] && exit 1

exclude_list="scene_switcher.hpp"

script_path=${0%/*}
tools_root=${script_path%/*}
project_root=$tools_root/..

if [[ "$0" != "$project_root" ]] && [[ "$project_root" != "" ]]; then
  cd "$project_root" || exit 1
  echo "-- Changed pwd to $(pwd)"
fi

exclude_cmd=
for exclude in $exclude_list; do
  exclude_cmd+="! -name $exclude"
done

files=$(find src/spaced src/android/app/src/main src/desktop -name "*.?pp" $exclude_cmd)

if [[ "$files" == "" ]]; then
  echo "-- No source files found"
  exit
fi

clang-format -i $files || exit 1

echo -e "-- Formatted Files:\n$files\n"

exit

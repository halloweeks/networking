#!/usr/bin/env bash
if ! dpkg -s cmake >/dev/null 2>&1; then
  echo -e "We not found cmake on your system\nPlease run apt-get install cmake"
  exit 1;
fi

if ! dpkg -s clang >/dev/null 2>&1; then
   echo -e "We not found clang on you system\nPlease run apt-get install clang"
fi

rm -rf build
mkdir build
cd build
cmake ..
make
cp server.exe ../
cp client.exe ../
rm -rf ../build
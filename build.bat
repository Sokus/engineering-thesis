@echo off

if not exist build mkdir build
pushd build
cmake .. -D DISTRIBUTE=OFF
cmake --build . --config Debug
popd
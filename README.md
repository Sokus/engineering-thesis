# Engineering Thesis
Repository containing base code of Engineering Thesis

## Building
Get the repository:
```sh
git clone --recursive https://github.com/Sokus/engineering-thesis
cd engineering-thesis
mkdir build && cd build
```
For development:
```sh
# while in the build directory
cmake .. -D DISTRIBUTE=OFF
cmake --build .
```
For distribution:
```sh
# while in the build directory:
cmake .. -D DISTRIBUTE=ON
cmake --build . --config Release
cmake --install .
```

## Authors
Kacper Piotrowski \
Krzysztof Jajeśnica \
Dawid Bosy

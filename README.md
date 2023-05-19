# Engineering Thesis
First "big" project I've worked on, also the first team coding experience. Almost everything was created from scratch - network layer, communication protocol, bit-level serialization, UI layouting and interaction, server code and most of the client application. Client app relies on [Raylib](https://www.raylib.com/index.html) library for creating the window, initializing OpenGL context and handling user input.

https://github.com/Sokus/engineering-thesis/assets/26815390/157f416d-9c89-4fee-865b-4e83239468da

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

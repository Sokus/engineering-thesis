# PracaInzynierska
Repository containing base code of Engineering Thesis
## Skład grupy
Kacper Piotrowski **144629** \
Krzysztof Weltrowski **144493** \
Krzysztof Jajeśnica  **145367** \
Dawid Bosy **145396**

## Kompilacja:
```sh
git clone --recursive https://github.com/DawidBosy/PracaInzynierska
cd PracaInzynierska
mkdir build && cd build
cmake .. -D DISTRIBUTE=OFF
cmake --build .
```

## Dystrybucja:
```sh
# while in the build directory:
cmake .. -D DISTRIBUTE=ON
cmake --build . --config Release
cmake --install .
```

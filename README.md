# PracaInzynierska
Repository containing base code of Engineering Thesis
## Code consistency
### Directory
Directory names shouldn't be shortcuts - the best option is one descriptive word, e.g.
**resources**
### Variable
Variables should start with small letter, and every new word begins with capital, and be written as noun, e.g. 
**lengthOfPlayerAttribute**
Boolean should be written as question in the infinitive (beacuse answer can be only true or false), e.g.
**isPlayerOnGround**
Constant variables defined before function and used multiple times should be written all capital with underscores, e.g. 
**SCREEN_WIDTH**
### Function
Function name, similiar as variable, start with small letter, and every new word begins with capital, however form should be as verb in infinitive, e.g.
**computePlayerJumpHeight**

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

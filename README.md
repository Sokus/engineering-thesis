# PracaInzynierska
Repository containing base code of Engineering Thesis

## Contents
- [Code Style Guide](#code-style-guide)
  - [Variables](#variables)
  - [Functions](#functions)
  - [Structs, Classes and Enums](#structs-classes-and-enums)
  - [Files and Directories](#files-and-directories)
- [Building](#building)
- [Authors](#authors)

## Code Style Guide
### Variables
Should be written in `camelCase` as a noun, e.g. **lengthOfPlayerAttribute**.
Use verbose names, avoid abbreviations.

**Booleans** should be written as questions in the infinitive (beacuse answer can be only true or false), e.g. **isPlayerOnGround**. \
**Constants** should be defined before function and used multiple times should be written all capital with underscores, e.g. **SCREEN_WIDTH**. \

### Functions
Should be written in `camelCase` as a verb in infinitive form, e.g. **computePlayerJumpHeight**.

### Structs, Classes and Enums
Should be written in `PascalCase`, e.g **CoreData**, **LogLevel**

### Files and Directories
Don't use abbreviation, use descriptive names. e.g. **resources**, **shaders** instead of **res**, **gfx**.

## Building
Get the repository:
```sh
git clone --recursive https://github.com/DawidBosy/PracaInzynierska
cd PracaInzynierska
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
Kacper Piotrowski **144629** \
Krzysztof Weltrowski **144493** \
Krzysztof Jajeśnica  **145367** \
Dawid Bosy **145396**

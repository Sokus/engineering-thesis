#!/bin/bash

# Figure out the script location (to use absolute paths)
script=$(readlink -f "$0")
location=$(dirname "$script")

# Common flags
warnings="-Wall -Wextra -Wshadow -Wconversion -Wdouble-promotion -Wno-unused-function"
warnings="$warnings -Wno-write-strings"
common="-O0 -g"
inc_dir="$location/external/include"

# Source files to compile
platform_src="$location/src/pi_linux_main.cpp"
sources="$platform_src"

external_flags="-pthread"

mkdir -p build
cd build

g++ $sources -o pi.out $common $warnings $external_flags 

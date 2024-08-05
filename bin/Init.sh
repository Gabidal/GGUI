#!/bin/bash

# Initializes and builds GGUI locally.

# Check if meson is installed or not
if ! command -v meson &> /dev/null
then
    echo "meson could not be found. Please install meson before running this script."
    exit
fi

# check if g++ is installed or not
if ! command -v g++ &> /dev/null
then
    echo "g++ could not be found. Please install g++ before running this script."
    exit
fi

# Check if g++ is in CXX variable
if [ -z "$CXX" ]; then
    # set CXX to value of 'g++'
    CXX=g++
fi

# check if there is ./Build folder
if [ -d "./Build" ]; then
    meson setup --wipe Build
else
    meson setup Build
fi

# Now compile
meson compile -C Build

echo "Done!"
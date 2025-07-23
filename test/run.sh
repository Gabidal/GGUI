#!/bin/bash

# First we need to make sure that the GGUI has exported
cd ./bin/export

g++ buildGGUILib.cpp    # build the exporter

./a.out # run the exporter

# Now the libGGUIUnix.a should exist

if [ -f "libGGUIUnix.a" ]; then
    echo "GGUI library exported successfully."
else
    echo "Error: GGUI library not found. Please check the export process."
    exit 1
fi

# Now we can build tester.cpp with the libGGUIUnix.a

cd ./../../ # Return back to root folder

cd ./test

g++ tester.cpp -g -o tester -L../bin/export -lGGUIUnix -I../bin/export

if [ $? -ne 0 ]; then
    echo "Error: Failed to compile tester.cpp. Please check the code and try again."
    exit 1
fi

./tester    # Run the tester

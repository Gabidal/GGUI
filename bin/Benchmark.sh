#!/bin/bash

# This bash script automaticaly creates an valgrind with callgrind support, 
# then opens it with kcachegrind and then asks the user if they want to preserve the previous kcachegrind output, 
# if so, then the script will add an .backup at the end of the file

# This script also takes one argument: -F/-f, which puts the profiling into max.

# check if -help was called
if [ "$1" = "-help" ] || [ "$1" = "--help" ] || [ "$1" = "-h" ] || [ "$1" = "--h" ]; then
    echo "This script is used to profile the GGUI project with valgrind and callgrind."
    echo "This script takes one argument: -F/-f, which puts the profiling into max."
    exit 0
fi

# First build a new build
echo "Building new build"

# get current dir
current_dir=$(pwd)

$current_dir/bin/Build.sh

# Since the valgrind will sadly remove build logs we need to add 1-3 seconds of sleep time for the user to be able to read
echo "Starting benchmark..."
sleep 3

# Now run the built GGUI with valgrind with callgrind enabled
echo "Running GGUI with valgrind with callgrind enabled"

# default valgrind settings: 
Default_Settings=" --tool=callgrind --dump-instr=yes -s "

Full_Settings=" --tool=callgrind --dump-instr=yes --collect-jumps=yes --simulate-cache=yes --collect-systime=yes "

# Now detemrine if the -F/-f arg was given
Current_Flag=""
if [ "$1" = "-F" ] || [ "$1" = "-f" ]; then
    Current_Flag=$Full_Settings
else
    Current_Flag=$Default_Settings
fi

valgrind $Current_Flag --callgrind-out-file=callgrind.out $current_dir/bin/Build/GGUI

# Now we need to open the newly made profile file with kcachegrind
echo "Opening the profile file with kcachegrind"

kcachegrind callgrind.out

# Now we can just wait until the user closes kcachegrind, when it does, just ask if the user wants to preserve the current profile, if no then just remove it. Default 'enter' to just remove it
echo "Do you want to preserve the current profile? [Y/n]"
read -r answer

if [ "$answer" = "Y" ] || [ "$answer" = "y" ]; then
    current_date=$(date '+%Y-%m-%d_%H-%M-%S')
    mv callgrind.out callgrind.out.$current_date.backup
else
    rm callgrind.out
fi


#!/bin/bash

directory="$1"

echo "Listing dirs in directory $directory"

dirlist=$(ls -d "$directory"/*/)

# check if dirlist is not empty
if [ -n "$dirlist" ]; then
    echo "Directories exist. Running the Python script on each directory."
    
    for dir in $dirlist
    do
        # If the directory name starts with a 2023, then execute the script
        # on the directory.
        if [[ $dir == *"2023"* ]]; then
            echo "Running script on $dir"
            python3 yolov8_detect.py "$dir"
        fi
    done
else
    echo "No directories found."
fi
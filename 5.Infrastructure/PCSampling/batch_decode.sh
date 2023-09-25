#!/bin/bash

directory="$1"
suffix="bin"

echo "Listing files with suffix .$suffix in directory $directory"

filelist=$(ls "$directory"/*."$suffix")

# check if filelist is not empty
if [ -n "$filelist" ]; then
    echo "Files exist with .$suffix extension. Running the Python script on each file."
    
    for file in $filelist
    do
        echo "Running script on $file"
        python3 decoder.py "$file"
    done
else
    echo "No files found with .$suffix extension."
fi
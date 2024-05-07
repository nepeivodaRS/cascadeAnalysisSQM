#! /usr/bin/env bash

directory="results"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

workingDir=\"${PWD}\"

root -l "../topoStudy1D.C(2,
             \"DATA.txt\",
             \"MC.txt\",
             \"R_{casc}\",
             0.4, 0.6,
             0.1, 1.5,
             $workingDir,
             \"_cascradius\")"
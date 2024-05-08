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
             \"DCA_{cascdau}\",
             0.7, 1.5,
             0.98, 1.01,
             0.99, 1.01,
             $workingDir,
             \"_dcacascdau\")"
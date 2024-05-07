#! /usr/bin/env bash

directory="results"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

workingDir=${PWD}

root -l "../topoStudy1D.C(2,
             \"casccospaDATA.txt\",
             \"casccospaMC.txt\",
             \"cos(PA)\",
             0.970,
             0.999,
             0.1,
             1.5,
             \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/topoStudy/casccospa\",
             \"_casccospa\")"
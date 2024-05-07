#! /usr/bin/env bash

root -l "generatejson_systematics.cpp"
cd data
./runVariedAnalysis.sh
./runVariedYields.sh
cd ../mc
./runVariedAnalysis.sh
./runVariedYields.sh
cd yieldsOut/
find "$(pwd)" -type f -name "*.root" > ../../MC.txt
cd ../../data/yieldsOut
find "$(pwd)" -type f -name "*.root" > ../../DATA.txt
cd ../../
./runTopoStudy.sh

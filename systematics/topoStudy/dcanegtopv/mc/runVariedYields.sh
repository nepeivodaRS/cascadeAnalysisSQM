#! /usr/bin/env bash

directory="yieldsOut"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

cut="dcanegtopv"

workingDir=\"${PWD}\"

pathToMC="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/data/25apr_lhc24b1/AnalysisResults.root\""

#default
root -l -b -q "../../../../yield.C(2, 0, 0, 1,
               $pathToMC,
               \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/topoStudy/$cut/mc/results/MC-LHC24b1-DEF.root\",
               $workingDir,
               \"_DEF\")"

#systematics
for i in {1..9}
do
    root -l -b -q "../../../../yield.C(2, 0, 0, 1,
                   $pathToMC,
                   \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/topoStudy/$cut/mc/results/MC-LHC24b1-$i.root\",
                   $workingDir,
                   \"_$i\")"        
done

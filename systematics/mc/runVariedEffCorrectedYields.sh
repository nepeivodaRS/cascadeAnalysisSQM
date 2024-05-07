#! /usr/bin/env bash

workingDir="\"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/mc\""

directory="yieldsOutEffCorr"
# Check if the directory exists
if [ -d "$directory" ]; then
    echo "Directory '$directory' exists."
else
    echo "Directory '$directory' does not exist."
    mkdir $directory
fi

#default
root -l -b -q "../../yieldEffCorr.C(2, 0, 0,
                  \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/efficiency_XiPm_DEF.root\",
                  $workingDir,
                  \"_DEF\")"

#current
root -l -b -q "../../yieldEffCorr.C(2, 0, 0,
                  \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/efficiency_XiPm_DEF.root\",
                  $workingDir,
                  \"_CURR\")"

#loosest
root -l -b -q "../../yieldEffCorr.C(2, 0, 0,
                  \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/efficiency_XiPm_LOOSEST.root\",
                  $workingDir,
                  \"_LOOSEST\")"

#systematics
for i in {1..2}
do
    root -l -b -q "../../yieldEffCorr.C(2, 0, 0,
                      \"/Users/rnepeiv/workLund/PhD_work/run3omega/cascadeAnalysisSQM/systematics/data/efficiencies/efficiency_XiPm_$i.root\",
                      $workingDir,
                      \"_$i\")"
done
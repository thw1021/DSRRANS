#!/bin/bash
dir=('case_0p5' 'case_0p8' 'case_1p0')
for i in "${!dir[@]}"; do
(
    [ -d ${dir[i]} ] && cd ${dir[i]} || exit

    . $WM_PROJECT_DIR/bin/tools/RunFunctions

    rm -rf log* postProcessing/ *0000/ processor* *.pdf *.csv

    # sed -i '/numberOfSubdomains/s/6/10/g' system/decomposeParDict

    application=$(getApplication)
    runApplication decomposePar
    runParallel $application

    runApplication reconstructPar -latestTime
    rm -r processor*
    runApplication -s sample  postProcess -func sample -latestTime
    runApplication foamLog log.simpleFoam

)
done

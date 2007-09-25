#!/bin/sh

templateCFG=template_UploadGainFromAsciiFile.cfg 

[ ! -e AsciiDir ] && mkdir AsciiDir
for mode in TIBTOB TIF
  do
  [ "$mode" == "TIBTOB" ] && listRun="6094 6445 6891 7222"
  [ "$mode" == "TIF" ] && listRun="8427 9148 9242 10212 10607 10821 11176 11311 12043"
  
  for run in ${listRun}
    do
    InputFile=tickmarks_dump_run_$run.txt
    cd AsciiDir
    [ ! -e ${InputFile} ] && wget http://cmstac00.cern.ch/commissioning_runs/${InputFile}
    cd -
    cat ${templateCFG} | sed -e "s@_insertTAG_@SiStripCabling_TIBTOB_v1@" -e "s@_insertRun_@$run@g" >> AsciiDir/cfgUploadGainFromAscii_${mode}_${run}.cfg
    eval `scramv1 runtime -sh`
    cmsRun AsciiDir/cfgUploadGainFromAscii_${mode}_${run}.cfg
    echo $run
  done
done

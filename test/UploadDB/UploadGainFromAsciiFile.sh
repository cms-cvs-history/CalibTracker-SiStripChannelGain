#!/bin/sh

templateCFG=template_UploadGainFromAsciiFile.cfg 
eval `scramv1 runtime -sh`

[ ! -e AsciiDir ] && echo -e "\nERROR:\n AsciiDir doesn't exist. Please create the AsciiDir, containing the tickmarks_dump_run_runNb.txt to be uploaded\nEXIT'n" && exit

cd AsciiDir
rm GainFromAsciiFile.db
cmscond_bootstrap_detector.pl --offline_connect sqlite_file:GainFromAsciiFile.db --auth /afs/cern.ch/cms/DB/conddb/authentication.xml STRIP

[ "`ls tickmarks_dump_run_*txt 2>/dev/null | wc -l`" == "0" ] && echo -e "\nNo tickmarks_dump_run_*txt in AsciiDir\nEXIT\n" && exit
 
for InputFile in `ls tickmarks_dump_run_*txt`
  do
  echo $InputFile
  declare -i run
  arun=`echo $InputFile | cut -d "_" -f4 | cut -d "." -f1 `
  run=`echo $arun | awk '{print int($0)}'`
  echo $run
  cat ../${templateCFG} | sed -e "s@_insertRun_@$run@g" -e "s@_inputFile_@$InputFile@g" > cfgUploadGainFromAscii_${run}.cfg
  cmsRun cfgUploadGainFromAscii_${run}.cfg
done

mv GainFromAsciiFile.db ..

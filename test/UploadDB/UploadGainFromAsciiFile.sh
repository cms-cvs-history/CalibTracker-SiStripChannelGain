#!/bin/sh

templateCFG=template_UploadGainFromAsciiFile.cfg 
eval `scramv1 runtime -sh`

[ ! -e AsciiDir ] && echo -e "\nERROR:\n AsciiDir doesn't exist. Please create the AsciiDir, containing the tickmarks_dump_run_runNb.txt to be uploaded\nEXIT'n" && exit

cd AsciiDir

##########################################################################################################################################

connectstringA="sqlite_file:GainFromAsciiFile.db"
connectstringB="oracle://orcon/CMS_COND_STRIP"
connectstring=${connectstringB}
USER=CMS_COND_STRIP
PASSWD=SSWDC3MCAI8HQHTC

rm GainFromAsciiFile.db
#cmscond_bootstrap_detector.pl --offline_connect sqlite_file:GainFromAsciiFile.db --auth /afs/cern.ch/cms/DB/conddb/authentication.xml STRIP


workdir=`pwd`
if [ ! -e $CMSSW_BASE/src/CondFormats/SiStripObjects/xml ]; then
    cd $CMSSW_BASE/src 
    cvs co CondFormats/SiStripObjects/xml 
    cd $workdir 
fi

if [ `echo ${connectstring} | grep -c sqlite` -ne 0 ]  || [[ `echo ${connectstring} | grep -c oracle` -ne 0  &&  "c$1" == "cforce" ]] ; then
    
    echo -e "\n-----------\nCreating tables for db ${connectstring} \n-----------\n"
    
    IsSqlite=1
    rm `echo ${connectstring} | sed -e "s@sqlite_file:@@"`

    #cmscond_bootstrap_detector.pl --offline_connect ${connectstring} --auth /afs/cern.ch/cms/DB/conddb/authentication.xml STRIP
    for obj in `ls $CMSSW_BASE/src/CondFormats/SiStripObjects/xml/*xml`
      do
      echo -e  "\npool_build_object_relational_mapping -f $obj   -d CondFormatsSiStripObjects -c ${connectstring}\n"
      pool_build_object_relational_mapping -f $obj   -d CondFormatsSiStripObjects -c ${connectstring} -u $USER -p $PASSWD

    done
fi
##########################################################################################################################################

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

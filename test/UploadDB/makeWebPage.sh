#!/bin/sh

function makeHtml(){
    thewidth=300

#Header
    echo "<html><head><title>Summary Page Gain</title></head>" 
    echo '<table cellpadding="2" cellspacing="2" border="0" width="100%" align="center">'
    echo '<tbody>'
    echo '<td valign="middle" align="center"><h4>Run<td  valign="middle" align="center"><h4>Fiber0<td  valign="middle" align="center"><h4>Fiber1<td  valign="middle" align="center"><h4>Fiber2'

    for file in `ls $webpath | grep png | cut -d "." -f 1`
      do
      [ "`echo $file | grep -c Fiber0`" == "1" ] && Run=`echo $file | cut -d "_" -f 3 | cut -d "_" -f 1` && echo -e '<tr>\n\n<td valign="middle" align="center"><h4><a name="'$Run'">'$Run'</a></h4>'
      htmlfile=${htmlpath}/$file
      echo '<td valign="middle" align="center">'
      echo '<a href="'${htmlfile}.png'"><img src="'${htmlfile}.png'" style="border: 0px solid ; width: '$thewidth'px; "></a><br><a href="'${htmlfile}.pdf'">pdf</a>  <a href="'${htmlfile}.xml'">svg</a>'
    done

    echo '  </tbody>'
    echo '</table>'
    echo '<hr><h4>Root File with distributions <a href="'${htmlpath}/Gains.root'">here</a>'
}

tag=""
[ "c$1" == "c" ] && echo -e "\nplease specify Gain tag" && exit
tag=$1

webpathBase=/data1/MonitorGain
webpath=${webpathBase}/$tag
webadd="http://cmstac11.cern.ch:8080"
export htmlpath=`echo $webpath | sed -e "s@/data1@$webadd@"`

webfile=$webpathBase/MonitorGain_$tag.html

[ ! -e $webpathBase ] && mkdir $webpathBase
[ ! -e $webpath ] && mkdir $webpath

cp TkMapGain_Run_*.* Gains.root $webpath/.

cd $webpathBase
makeHtml > ${webfile}
cd -

if [ "$3" != "" ]
then
    export PATH=$3:$PATH
fi
section_name=`cat $1 | grep "\[" | tr -d "[" | tr -d "]"`
major_version=`cat $1 | grep "MAJOR_VERSION" | awk -F "=" '{printf "%d",$2}'`
minor_version=`cat $1 | grep "MINOR_VERSION" | awk -F "=" '{printf "%d",$2}'`
debug_version=`cat $1 | grep "DEBUG_VERSION" | awk -F "=" '{printf "%d",$2+1}'`
echo "#ifndef __H_$section_name" > $2
echo "#define __H_$section_name" >> $2
echo "" >>$2
echo "#define     MAJOR_VERSION    $major_version" >> $2
echo "#define     MINOR_VERSION    $minor_version" >> $2
echo "#define     DEBUG_VERSION    $debug_version" >> $2
echo "" >> $2
echo "#endif" >> $2
echo "" >> $2
sleep 2
echo "[$section_name]" > $1
echo "MAJOR_VERSION = $major_version" >> $1
echo "MINOR_VERSION = $minor_version" >> $1
echo "DEBUG_VERSION = $debug_version" >> $1
echo "" >> $1




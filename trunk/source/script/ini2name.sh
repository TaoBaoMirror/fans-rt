if [ "$3" != "" ]
then
    export PATH=$3:$PATH
fi
cat $1  | tr -d '\r' > $1.tmp
section_name=`cat $1.tmp | grep "\[" | tr -d "[" | tr -d "]"`
macro_suffix_name="_NAME_H"
array_suffix_name="NameString"
echo "#ifndef __$section_name$macro_suffix_name" > $2
echo "#define __$section_name$macro_suffix_name" >> $2
echo "" >>$2
echo "static const char * g_$section_name$array_suffix_name[] = {" >> $2
cat $1.tmp | grep "=" | awk -F ',' '{if ($2 != "") printf "    %s,\n",$2}'>> $2
echo "};" >> $2
echo "" >> $2
echo "#endif" >> $2
echo "" >> $2
rm -f $1.tmp

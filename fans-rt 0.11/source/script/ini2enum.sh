if [ "$3" != "" ]
then
    export PATH=$3:$PATH
fi
cat $1  | tr -d '\r' > $1.tmp
section_name=`cat $1.tmp | grep "\[" | tr -d "[" | tr -d "]"`
macro_suffix_name="_ENUM_H"
enum_perfix_name="E_"
enum_suffix_name="_DEFINE"
echo "#ifndef __$section_name$macro_suffix_name" > $2
echo "#define __$section_name$macro_suffix_name" >> $2
echo "" >>$2
echo "typedef enum{" >> $2
cat $1.tmp | grep "=" | awk -F '=' '{if ($2 != "") printf "    %s = %s ,\n",$1,$2}' |\
     awk -F ',' '{printf "%s,\n",$1}' >> $2
echo "}$enum_perfix_name$section_name$enum_suffix_name;" >> $2
echo "" >> $2
echo "#endif" >> $2
echo "" >> $2
rm -f $1.tmp

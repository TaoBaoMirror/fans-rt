if [ "$3" != "" ]
then
    export PATH=$3:$PATH
fi
cat $1  | tr -d '\r' > $1.tmp
section_name=`cat $1.tmp | grep "\[" | tr -d "[" | tr -d "]"`
echo "#ifndef __H_$section_name" > $2
echo "#define __H_$section_name" >> $2
echo "" >>$2
echo "typedef enum{" >> $2
cat $1.tmp | grep "=" | awk -F "=" '{printf "    %s = %s,\n",$1,$2}' >> $2
echo "}$section_name;" >> $2
echo "" >> $2
echo "#endif" >> $2
echo "" >> $2
rm -f $1.tmp

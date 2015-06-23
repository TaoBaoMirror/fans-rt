if [ "$3" != "" ]
then
    export PATH=$3:$PATH
fi
cat $1  | tr -d '\r' > $1.tmp
echo "" >$2
cat $1.tmp | grep "=" | awk -F "," '{printf "%s\n",$1}' |  awk -F "=" '{printf ".equ %s, %s\n",$1,$2}'>> $2
echo "" >>$2
rm -f $1.tmp

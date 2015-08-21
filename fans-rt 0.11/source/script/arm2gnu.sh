if [ "$2" == "" ]
then
    export o_file=$1.gnu.s
else
	export o_file=$2
fi
i_file=$1
cat $1 | awk -F ';' '{printf "%s\n",$1}' \
    | awk -F ' ' '{
    if ($2 == "PROC") printf "    .type %s, %%function\n%s:\n",$1,$1;
    else if ($2 == "EQU") printf "    .equ %s, %s\n",$1,$3;
    else if ($1 == "END") printf ".end\n";
    else if ($1 == "ENDP") printf "\n";
    else if ($1 == "AREA") printf "    .section .text\n";
    else if ($1 == "INCLUDE") printf ".include %c%s%c\n",34,$2,34;
    else if ($1 == "THUMB") printf "    .thumb\n";
    else if ($1 == "ALIGN") printf "    .align %s\n",$2;
    else if ($1 == "PRESERVE8") printf "   .syntax unified";
    else if ($1 == "EXPORT") printf ".global %s\n",$2;
    else if ($1 == "IMPORT") printf ".extern %s\n",$2;
    else if ($9 != "") printf "    %-4s %-4s %-4s %-4s %-4s %-4s %-4s %-4s %-4s\n",$1,$2,$3,$4,$5,$6,$7,$8,$9;
    else if ($8 != "") printf "    %-4s %-4s %-4s %-4s %-4s %-4s %-4s %-4s\n",$1,$2,$3,$4,$5,$6,$7,$8;
    else if ($7 != "") printf "    %-4s %-4s %-4s %-4s %-4s %-4s %-4s\n",$1,$2,$3,$4,$5,$6,$7;
    else if ($6 != "") printf "    %-4s %-4s %-4s %-4s %-4s %-4s\n",$1,$2,$3,$4,$5,$6;
    else if ($5 != "") printf "    %-4s %-4s %-4s %-4s %-4s\n",$1,$2,$3,$4,$5;
    else if ($4 != "") printf "    %-4s %-4s %-4s %-4s\n",$1,$2,$3,$4;
    else if ($3 != "") printf "    %-4s %-4s %-4s\n",$1,$2,$3;
    else if ($2 != "") printf "    %-4s %-4s\n",$1,$2;
    else if ($1 != "") printf "%s:\n",$1;
    else printf "\n";
    }'  > $o_file


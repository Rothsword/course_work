#!/bin/bash
# Call this script testarg and try out different situations 
echo "This script is called $0"
if [ $# -eq 0 ]; then
    echo "No argument"
else
    n=$(($#-1))
fi

for((i = 0; i < n; i++)); do
    eval var=\${$(($i + 1))}
    eval arrays[$i]=$var
done

for item in "${arrays[@]}"
do
    eval declare -a array$item
    eval array$item[0]=$item
    eval array$item[1]=0
    eval array$item[2]=0
    eval array$item[3]=0
    eval array$item[4]=0
    eval array$item[5]=0
    eval array$item[6]=0
    eval array$item[7]=0
    eval array$item[8]=0
    
done

getArray() {
    array=() # Create array
    while IFS= read -r line # Read a line
    do
        array+=("$line") # Append line to the array
    done < "$1"
}


for((i = 1; i <= n*2; i++)); do
    getArray "${!#}$i.txt"
    arr=(${array[0]})
    #eval var=\${array$j[0]}
    for p in "${!arr[@]}" 
    do
        eval j=\${arr[$p]}
        if [ $(($p % 2)) == 0 ]; then
            eval games=\${array$j[1]}
            eval array$j[1]=$(($games + 1))
            eval goals=\${arr[$(($p + 1))]}
            #echo ${arr[$p]}
            #echo $goals
            eval sumgoals=\${array$j[5]}
            eval array$j[5]=$(($goals + $sumgoals))
            if [ $(($p % 4)) == 0 ]; then
                eval lost=\${arr[$(($p + 3))]}
            elif [ $(($p % 2)) == 0 ]; then
                eval lost=\${arr[$(($p - 1))]}
            fi
            eval sumlost=\${array$j[6]}
            eval array$j[6]=$(($lost + $sumlost))
            if [ $goals -gt $lost ]; then
                eval win=\${array$j[2]}
                eval array$j[2]=$(($win + 1))
            elif [ $goals -lt $lost ]; then
                eval lose=\${array$j[4]}
                eval array$j[4]=$(($lose + 1))
            elif [ $goals -eq $lost ]; then
                eval draw=\${array$j[3]}
                eval array$j[3]=$(($draw + 1))
            fi
            eval templost=\${array$j[6]}
            eval tempgoals=\${array$j[5]}
            eval array$j[7]=$(($tempgoals - $templost))
            eval tempwin=\${array$j[2]}
            eval tempdraw=\${array$j[3]}
            eval templose=\${array$j[4]}
            eval array$j[8]=$(($(($tempwin * 3)) + $(($tempdraw * 1))))
        fi
    done
done

#echo ${arrayqatar[@]}
#eval arraychina[8]=21
#eval arraychina[7]=25
#eval arraychina[5]=40

printf "%s  %8s %8s    %6s %8s    %6s %8s %8s %8s   %8s" Rank Team P W D L GF GA GD Points
printf "\n"

q=1
for items in "${arrays[@]}"
do
    eval j=\${array$items[@]}
    m=($j)
    #echo ${m[1]}
    #echo $(($(($n - 1)) * 2))
    if [ ${m[1]} -lt $(($(($n - 1)) * 2)) ]; then
        dif=$(($(($(($n - 1)) * 2))-${m[1]}))
        printf "%s %s %s %s" ${m[0]} ${m[5]} ${m[7]} $((${m[8]}+$dif)) >>13103693dl.txt
    else       
        printf "%s %s %s %s" ${m[0]} ${m[5]} ${m[7]} ${m[8]} >>13103693dl.txt
    fi
    printf "\n" >>13103693dl.txt
done

sort -nrk4 -nrk3 -nrk2 13103693dl.txt -o 13103693dl.txt
getArray "13103693dl.txt"
rm 13103693dl.txt
for p in "${array[@]}"
do
    arr=($p)
    k=${arr[0]}
    eval j=\${array$k[@]}
    m=($j)
    if [ ${m[1]} -lt $(($(($n - 1)) * 2)) ]; then
        dif=$(($(($(($n - 1)) * 2))-${m[1]}))
        printf "%3s   %8s %8s%s %6s %8s%s %6s %8s %8s %8s %8s%s" $q ${m[0]} $((${m[1]}+$dif)) "(${m[1]})" ${m[2]} $((${m[3]}+$dif)) "(${m[3]})" ${m[4]} ${m[5]} ${m[6]} ${m[7]} $((${m[8]}+$dif)) "(${m[8]})"
        q=$(($q+1))
    else       
        printf "%3s   %8s %8s    %6s %8s    %6s %8s %8s %8s %8s" $q ${m[0]} ${m[1]} ${m[2]} ${m[3]} ${m[4]} ${m[5]} ${m[6]} ${m[7]} ${m[8]}
        q=$(($q+1))
    fi
    printf "\n"
done


    



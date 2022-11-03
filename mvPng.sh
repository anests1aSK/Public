#!/bin/bash

cd $(dirname ${0})

varCount=0
string=""

for file in $(ls ); do
     if [[ ${string} =~ ^Screen ]]; then

        if [[ ! ${varCount} -eq 3 ]]; then
        string+=" ${file}"
        ((varCount+=1)) 
        fi
        if [[ ${varCount} -eq 3 ]]; then
            echo "${string}" | grep --quiet '2022-10-14'
            if [[ $? -eq 0 ]]; then 
               var=$(echo "${string}" | sed 's/ /\\ /g')              
               mv "${var}" ~/Desktop            
            fi
            varCount=0
            string=""
        fi
    fi

    if [[ ${file} =~ ^Screen ]]; then
         string+="${file}"
    fi
    
    
done



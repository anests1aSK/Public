#!/bin/bash

cd $(dirname $0)

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <File_Name>" 
    exit 1
#elif [[ ! -d ${1} && ! -f ${1} ]]; then
   # echo "This program only works with <directory>, <files>"
   # exit 1
fi

find / -name "${1}" 2> /dev/null

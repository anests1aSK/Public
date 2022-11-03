#!/bin/bash

file_name=$1

if [[ -f ${file_name} ]];then
    cp /dev/null ${file_name}
else
    touch ${file_name}
fi

if [[ $? -ne 0 ]]; then
    echo "Error please check file"
    exit 1
else
    echo "Success the ${file_name} create empty file"
fi


#!/bin/bash

if [[ $# -ne 3 ]]; then 
    echo "Usage: $0 <directory_name> <file_name> <file_content>"
    exit 1
fi

directory_name=${1}
file_name=${2}
file_content=${3}

absoluted_file_path=${directory_name}/${file_name}

if [[ ! -d ${directory_name} ]]; then
    mkdir ${directory_name} || { echo "Cannot create file directory"; exit 1; }
fi

if [[ ! -f ${absoluted_file_path} ]]; then 
    touch ${absoluted_file_path} || { echo "Cannot create file"; exit 1; }
fi

echo "${file_content}" > ${absoluted_file_path}

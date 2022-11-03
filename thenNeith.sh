#!/bin/bash

cd $(dirname $0)

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <file_path>."
    exit 1
fi

input_file=${1}

if [[ -r ${input_file}  ]]; then
    if [[ -f ${input_file}  ]]; then 
        echo "Print File"
        cat ${input_file}
    elif [[ -d ${input_file} ]]; then
        echo "Listing directory"
        ls ${input_file}
    else 
        echo "Path is neither a file nor a directory, exiting script"
        exit 1
    fi
else 
    echo "Cannot read the file/directory, exiting script"
    exit 1
fi


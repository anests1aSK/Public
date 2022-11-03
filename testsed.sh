#!/bin/bash

cd $(dirname $0)

if [[ !  $# -eq 1 ]]; then
    echo "Usage: $0 <directory/file>"
    exit 1
elif [[ -d ${1} ]]; then    
    for file in $(ls ${1}); do
        echo --quiet ${file} | grep --quiet -i -e '.\txt' -e '.\log' 
        if [[ $? -eq 0 ]]; then
            grep --quiet -i 'error' ${1}/${file}
            if [[ $? -eq 0 ]]; then
                sed --quiet --in-place '/[eE][rR][rR][oO][rR]/p' ${1}${file}
            fi
        fi
    done
fi

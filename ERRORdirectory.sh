#!/bin/bash

ERROR_DIRECTORY='/tmp/ERROR_logs'

mkdir -p ${ERROR_DIRECTORY}

for file in $(ls /var/log/*.log); do
    grep --quiet -i 'error' ${file}    
    if [[ $? -eq 0 ]]; then        
        echo "${file} contains error(s), copying it to archive."
        cp ${file} ${ERROR_DIRECTORY}
        file_new_location="${ERROR_DIRECTORY}/$(basename ${file})"
        sed --quiet --in-place '/[Ee]rror/p' ${file_new_location}                     
    fi
done


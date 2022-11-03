#!/bin/bash

FILE=/tmp/grepfile.txt
KEYWORD=${1}

if [[ ! $# -eq 1  ]]; then
    echo "Usange $0: <keyword>"
fi

if [[ ! -f ${FILE} ]]; then
    touch ${FILE} 
fi

grep -q "${KEYWORD}" ${FILE} 
grep_rc=$?

if [[ ${grep_rc} -eq 0 ]]; then
    rm ${FILE} || { echo "Error removing file."; exit 1; }
else
    echo "${KEYWORD}" >> ${FILE}
fi

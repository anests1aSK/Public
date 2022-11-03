#!/bin/bash

FILE=/tmp/ramdom_file.txt

if [[ ! -f ${FILE} ]]; then
    echo "FIle does not exit, sorry"
    exit 1
fi

cat ${FILE}

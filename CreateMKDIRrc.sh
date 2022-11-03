#!/bin/bash

DIRECTORY=/home/anests1a/Desktop/NewDIRECTORY

mkdir $DIRECTORY
mkdir_rc=$?

if [[ ${mkdir_rc} -ne 0 ]]; then
    echo "mkdir not successfully complete, sorry"
    exit 1
fi

touch ${DIRECTORY}/newFile.txt


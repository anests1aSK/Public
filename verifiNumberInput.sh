#!/bin/bash

INPUT=${1}
MSG_ERROR_INPUT="Error input. Usage $0 <number>"

if [[ $# -ne 1 ]]; then
    echo ${MSG_ERROR_INPUT}
    exit 1
fi

if [[ ! ${INPUT} =~ ^[0-9]{1,}$ ]]; then
    echo ${MSG_ERROR_INPUT}
    exit 1
else
   echo $((${INPUT} * ${INPUT}))
fi

#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <length>"
    exit 1
fi

if [[ ! ${1} =~ ^[0-9]+$  ]]; then
    echo "Please enter a length (number)"
    exit 1
fi

password_length=${1}

tr -dc 'a-zA-Z0-9' < /dev/urandom | head -c ${password_length}
echo

#!/bin/bash

for file in $(ls /var/log/*.log);do
    echo "File: ${file}"
    grep -i 'error' ${file}
done

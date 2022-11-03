#!/bin/bash

DIRECTORY=/tmp/

test -d ${DIRECTORY}
test_rc=$?

[ -d ${DIRECTORY} ]
test2_rc=$?

[[ -d ${DIRECTORY} ]]
test3_rc=$?

echo "test_rc = ${test_rc}, test2_rc = ${test2_rc}, test3_rc = ${test3_rc}"


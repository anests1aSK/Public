#!/bin/bash

character_name=${1}
pet_name=${2}
mother_name=${3}

if test -z ${character_name}; then read -p "Enter your name " character_name; fi
if test -z ${pet_name}; then read -p "Enter you pet name " pet_name; fi
if test -z ${mother_name}; then read -p "Enter your mother name " mother_name; fi

echo "hello my character name is ${character_name}, my pet name is ${pet_name}, my mother name is ${mother_name}"



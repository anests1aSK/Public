#!/bin/bash

check_arguments() {
 
  if [[ $# -lt 1 ]]; then
    echo "Less than 1 argument received, exiting."
    exit 1
  fi  

  expected_arguments=$1

  shift 1 

  if [[ ${expected_arguments} -ne $# ]]; then
    return 1 
  fi
}


check_integer() {
  
  if [[ $# -ne 1 ]]; then
    echo "Need exactly one argument, exiting."
    exit 1 
  fi


  if [[ $1 =~ ^[[:digit:]]+$ ]]; then
    return 0
  else
    return 1
  fi
}


check_yes_no() {
  
  if [[ $# -ne 1 ]]; then
    echo "Need exactly one argument, exiting."
    exit 1.
  fi


  if [[ ${1,,} = 'y' || ${1,,} = 'yes' ]]; then
    return 0
  elif [[ ${1,,} = 'n' || ${1,,} = 'no' ]]; then
    return 1
  else
    echo "Neither yes or no, exiting."
    exit 2
  fi
}


set_cwd() {
  cd $(dirname $0)
}

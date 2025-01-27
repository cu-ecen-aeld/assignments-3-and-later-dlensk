#!/bin/bash

#Read-in two arguments
writefile=$1
writestr=$2

#Check for null arguments
if [[ -z "$writefile" ]] || [[ -z "$writestr" ]]
  then
    echo "Specify a filepath and string to write."
  exit 1
fi

#Create directory and write string to file
dir=$(dirname $writefile)
mkdir -p $dir 
echo "$writestr" > "$writefile" 

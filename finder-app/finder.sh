#!/bin/sh

#Read-in directory and search string
filedir=$1
searchstr=$2

#Check for null arguments
if [ -z "$filedir" ] || [ -z "$searchstr" ]
  then
    echo "Specify directory and search string."
  exit 1
fi

#Check if directory exists
if [ ! -d "$filedir" ]
  then
    echo "$filedir is not a directory"
  exit 1
fi

#Get file number and matching lines
numfiles=$(find "$filedir" -type f  | wc -l)
matchedlines=$(grep -rF "$searchstr" "$filedir" | wc -l )

echo "The number of files are $numfiles and the number of matching lines are $matchedlines"

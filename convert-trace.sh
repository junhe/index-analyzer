#!/bin/bash
# This script is used to convert a trace (plfs_map output) to the format that I can use in my index-analyzer.


echo "File to be converted: $1"
ifile=$1
sed -i 's/\.\ /\ /g' $ifile
sed -i 's/\[/\ /g' $ifile
sed -i 's/\]/\ /g' $ifile

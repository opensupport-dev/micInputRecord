#!/bin/bash 
printf "< Name of script : %s >\n" $0
echo "---------------------------"
echo "Started to build project..."
echo "---------------------------"

g++ -o micInput main.cpp -O2 -lpthread -lstdc++ -lasound -Wno-write-strings 

echo "----------------------------"
echo "Finished to build project..."
echo "----------------------------"
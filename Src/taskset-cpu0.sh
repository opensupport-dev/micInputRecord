#!/bin/bash 

#taskset -c 0,1,2,3,4 ./micInput
taskset -c 0 ./micInput

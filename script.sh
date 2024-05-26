#!/bin/bash

rm logs
make clean dynamic
LD_PRELOAD=/home/hanako/2600/my_malloc_/libmy_secmalloc.so ls 
#cat logs

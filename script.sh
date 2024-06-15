#dj:wq
:wq
:wq
/bin/bash

rm logs
make clean dynamic
LD_PRELOAD=/home/hanako/Project/my_malloc/libmy_secmalloc.so ls 
#cat logs

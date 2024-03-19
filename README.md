### Memory layout
RAM | purpose
-------------
2MB - 3MB  (0x200000 -  0x2fffff) | kernel
0x300000 | stack
3MB - 4MB  (0x300000 -  0x3fffff) | Memory for malloc
4MB - 20MB (0x400000 - 0x13fffff) | Image of disk

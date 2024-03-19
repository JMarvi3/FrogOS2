#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "fs.h"

int main(int argc, char **argv)
{
unsigned long num_blocks = 1*1024; // 1MB
unsigned short num_inodes = 128;
if(argc == 2) num_blocks = atol(argv[1]);

printf("num_blocks: %ld\n", num_blocks);
unsigned short num_inode_blocks = num_inodes * sizeof(inode_t) / 1024;

int fd = open("fs.img", O_CREAT|O_WRONLY|O_TRUNC);
ftruncate(fd, num_blocks*1024);
fchmod(fd, 0666);
close(fd);

fd = open("fs.img", O_RDWR);
void *fs = mmap(NULL, num_blocks*1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

superblock_t *superblock = fs;
superblock->number_of_blocks = num_blocks;
superblock->number_of_inodes = num_inodes;;
superblock->inode_start = 2;
superblock->data_start = 2 + num_inode_blocks;
superblock->free_inode_list = 1;
superblock->free_data_list = superblock->data_start;

free_inode_t *free_inodes = fs + superblock->inode_start * 1024;
for(int i=1; i < superblock->number_of_inodes-1; ++i) {
	free_inodes[i].next_node = i+1;
}
free_inodes[superblock->number_of_inodes-1].next_node = -1;

free_data_t *free_data = fs;
for(int i=superblock->data_start; i<num_blocks-1; ++i) {
	free_data[i].next_node = i+1;
}
free_data[num_blocks-1].next_node = -1; 

return close(fd);
}

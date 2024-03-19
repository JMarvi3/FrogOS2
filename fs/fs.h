#include <stdint.h>
typedef struct {
	uint32_t number_of_blocks;
	uint16_t number_of_inodes;
	uint32_t inode_start;
	uint32_t data_start;
	uint32_t free_inode_list; // -1 is last node
	uint32_t free_data_list;
} __attribute__((packed)) superblock_t;

typedef struct {
	uint8_t  type;
	uint8_t  n_links;
	uint16_t reserved;
	uint32_t mod_time;
	uint32_t size;
	uint32_t blocks[5];
} __attribute__((packed)) inode_t;

typedef struct {
	uint32_t next_node;
	uint8_t padding[12];
} __attribute__((packed)) free_inode_t;

typedef struct {
	uint32_t next_node;
	uint8_t padding[1020];
} __attribute__((packed)) free_data_t;

typedef struct {
	uint16_t inode;
	uint8_t  type;
	unsigned char name[61];
} __attribute__((packed)) dirent_t;

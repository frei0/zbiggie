#ifndef _ZBIGFS_H
#define _ZBIGFS_H
#include "types.h"

#define FTYPE_SPECIAL   0
#define FTYPE_DIRECTORY 1
#define FTYPE_REGULAR   2

typedef struct dentry{
	char fname[32];
	uint32_t ftype;
	uint32_t inode_num;
	char reserved[24];
} dentry_t;

struct FILE;

typedef int (*TypedFileOperation) (struct FILE * f, void* buf, uint32_t count);

typedef struct FILE{
	TypedFileOperation * optable;
	uint32_t inode;
	int32_t pos;
	int32_t flags;
} FILE;
int zbigfs_mount(void * base_addr);
int kopen(FILE*, const int8_t * name);
int kread (FILE * f, void * buf, uint32_t numbytes );
int kwrite(FILE * f, void * buf, uint32_t numbytes );
int kclose(FILE * f);

int32_t read_dentry_by_name (const int8_t * fname, dentry_t * dentry); 
int32_t read_dentry_by_index (uint32_t index, dentry_t * dentry); 
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);
#endif

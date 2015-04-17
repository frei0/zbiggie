#include "zbigfs.h"
#include "lib.h"
#include "rtc.h"

#define ZBIGFS_BLOCK_SIZE 4096
#define FILEOP_IDX_OPEN  0
#define FILEOP_IDX_READ  1
#define FILEOP_IDX_WRITE 2

typedef struct boot_block{ //this struct is large, never allocate it!
	uint32_t num_dentry;
	uint32_t num_inode;
	uint32_t num_dblock;
	char reserved[sizeof(dentry_t)-3*sizeof(uint32_t)];
dentry_t dentries[ZBIGFS_BLOCK_SIZE/sizeof(dentry_t)-1];
} boot_block_t;

typedef struct inode{ //this struct is large, never allocate it!
	uint32_t length; //the length of the file
	uint32_t datablock_num[ZBIGFS_BLOCK_SIZE/sizeof(uint32_t)-1];
} inode_t;
boot_block_t * zbigfs_start;

int regular_read(FILE * f, void * buf, int32_t nbytes){
    nbytes = read_data(f->inode, f->pos, buf, nbytes); 
    if (nbytes < 0 ) return -1;
    f->pos+=nbytes;
    return nbytes;
}
int directory_read(FILE * f, void * buf, int32_t nbytes){
    dentry_t dentry;
    if (read_dentry_by_index(f->pos, &dentry)) return 0;
    f->pos++;
    strncpy(buf, dentry.fname, nbytes);
    return nbytes;
}
int readonly_write(FILE* f) { return -1; } //always fails 
int regular_open(FILE* f){ return 0;} //check if the file exists and update the FILE inode ptr
int directory_open(FILE* f){ return 0;} //check if the directory exists

TypedFileOperation ops_regular[3] = {(TypedFileOperation) regular_open, (TypedFileOperation) regular_read, (TypedFileOperation) readonly_write};
TypedFileOperation ops_directory[3] = {(TypedFileOperation) directory_open, (TypedFileOperation) directory_read, (TypedFileOperation) readonly_write};
TypedFileOperation ops_rtc[3] = {(TypedFileOperation) rtc_open, (TypedFileOperation) rtc_read, (TypedFileOperation) rtc_write};

int kopen (FILE * f, const int8_t * name){
    dentry_t dentry;
    if (read_dentry_by_name(name, &dentry)) return -1;
    f->pos = 0;
    switch (dentry.ftype){
        case FTYPE_REGULAR:
            f->inode = dentry.inode_num;
            f->optable = ops_regular;
            break;
        case FTYPE_SPECIAL:
            f->optable = ops_rtc;
            break;
        case FTYPE_DIRECTORY:
            f->optable = ops_directory;
    }
    return (*(f->optable[FILEOP_IDX_OPEN]))(f, NULL, 0);
}
int kread (FILE * f, void * buf, uint32_t numbytes ){ return (*(f->optable[FILEOP_IDX_READ ]))(f,buf,numbytes);}
int kwrite(FILE * f, void * buf, uint32_t numbytes ){ return (*(f->optable[FILEOP_IDX_WRITE]))(f,buf,numbytes);}
int kclose(FILE * f){return 0;}

int32_t read_dentry_by_name (const int8_t * fname, dentry_t * dentry){
	int i;
	for (i = 0; i < zbigfs_start->num_dentry; ++i){
		if (0==strncmp(fname, zbigfs_start->dentries[i].fname, FNAME_MAX_LEN-1)){
			return read_dentry_by_index(i,dentry);
		}
	}
	return -1; //not found
}
int32_t read_dentry_by_index (uint32_t i, dentry_t * dentry){
	if (i >= zbigfs_start->num_dentry) return -1;
	strncpy(dentry->fname, zbigfs_start->dentries[i].fname, FNAME_MAX_LEN);
	dentry->ftype = zbigfs_start->dentries[i].ftype;
	if (dentry->ftype == FTYPE_REGULAR){
		dentry->inode_num = zbigfs_start->dentries[i].inode_num;
	}
	return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length){
	if (inode >= zbigfs_start->num_inode) return -1;
	inode_t * inode_entry = (inode_t *)(zbigfs_start +1+ inode);
	char * datablock0 = (char *) (zbigfs_start+1+ zbigfs_start->num_inode);
	uint32_t first = offset;
	uint32_t last = offset + length;
	if (last > inode_entry->length) last = inode_entry->length;
	if (last < first) return 0;
	while (offset/ZBIGFS_BLOCK_SIZE < last/ZBIGFS_BLOCK_SIZE){
		memcpy(buf, datablock0 + ZBIGFS_BLOCK_SIZE * inode_entry->datablock_num[offset/ZBIGFS_BLOCK_SIZE] +
                offset%ZBIGFS_BLOCK_SIZE, ZBIGFS_BLOCK_SIZE - (offset%ZBIGFS_BLOCK_SIZE));
		buf += ZBIGFS_BLOCK_SIZE - (offset%ZBIGFS_BLOCK_SIZE);
		offset += ZBIGFS_BLOCK_SIZE - (offset%ZBIGFS_BLOCK_SIZE);
	}
	memcpy(buf, datablock0 + ZBIGFS_BLOCK_SIZE * inode_entry->datablock_num[offset/ZBIGFS_BLOCK_SIZE] + 
            offset%ZBIGFS_BLOCK_SIZE, last%ZBIGFS_BLOCK_SIZE - (offset%ZBIGFS_BLOCK_SIZE));
	return last - first;
}

int zbigfs_mount(void* base_addr){
	zbigfs_start = base_addr;
    //printf("%d %d %d \n", zbigfs_start->num_dentry, zbigfs_start->num_inode, zbigfs_start->num_dblock);
    return 0;
}

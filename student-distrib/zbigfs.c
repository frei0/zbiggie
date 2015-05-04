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

/* 
 * regular_read
 *   DESCRIPTION: reads from a file in the filesystem
 *   INPUTS:  f      - the FILE to read from
 *            nbytes - the number of bytes to read
 *   OUTPUTS: buf    - the buffer to read to
 *   RETURN VALUE: the number of bytes actually read.
 */
int regular_read(FILE * f, void * buf, int32_t nbytes){
    nbytes = read_data(f->inode, f->pos, buf, nbytes); 
    if (nbytes < 0 ) return -1;
    f->pos+=nbytes;
    return nbytes;
}

/* 
 * directory_read
 *   DESCRIPTION: reads from the directory listing in the filesystem
 *   INPUTS:  f      - the FILE to read from
 *            nbytes - the number of bytes to read
 *   OUTPUTS: buf    - the buffer to read to
 *   RETURN VALUE: the number of bytes actually read.
 */
int directory_read(FILE * f, void * buf, int32_t nbytes){
    dentry_t dentry;
    if (read_dentry_by_index(f->pos, &dentry)) return 0;
    f->pos++;
    strncpy(buf, dentry.fname, nbytes);
    return nbytes;
}

/* readonly_write
 *   DESCRIPTION: attempt to write to the filesystem. Always fails because its readonly.
 *   RETURN VALUE: the number of bytes actually read.
 */
int readonly_write(FILE* f) { return -1; }

/* regular_open
 *   DESCRIPTION: open a regular file. Do nothing because there is nothing to check at this point.
 *   RETURN VALUE: the number of bytes actually read.
 */
int regular_open(FILE* f){ return 0;}

/* directory_open
 *   DESCRIPTION: open a directory. Do nothing because there is nothing to check at this point.
 *   RETURN VALUE: the number of bytes actually read.
 */
int directory_open(FILE* f){ return 0;} //check if the directory exists

TypedFileOperation ops_regular[3] = {(TypedFileOperation) regular_open, (TypedFileOperation) regular_read, (TypedFileOperation) readonly_write};
TypedFileOperation ops_directory[3] = {(TypedFileOperation) directory_open, (TypedFileOperation) directory_read, (TypedFileOperation) readonly_write};
TypedFileOperation ops_rtc[3] = {(TypedFileOperation) rtc_open, (TypedFileOperation) rtc_read, (TypedFileOperation) rtc_write};

/* 
 * kopen
 *   DESCRIPTION: open a file by name, set up the file for 
 *                read/writing based on its type
 *   INPUTS:  f      - the FILE struct to store file info in
 *            name   - the filename
 *   RETURN VALUE: 0 on success, -1 otherwise.
 */
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

/* 
 * kread
 *   DESCRIPTION: read from a file. generic function which looks up the correct operation
 *                based on filetype. 
 *   INPUTS:  f        - the FILE to read from
 *            numbytes - the number of bytes to read
 *   OUTPUTS: buf      - the buffer to read to
 *   RETURN VALUE: number of bytes on success, -1 otherwise.
 */
int kread (FILE * f, void * buf, uint32_t numbytes ){ return (*(f->optable[FILEOP_IDX_READ ]))(f,buf,numbytes);}

/* 
 * kwrite
 *   DESCRIPTION: write to a file. generic function which looks up the correct operation
 *                based on filetype. 
 *   INPUTS:  f        - the FILE to write to
 *            buf      - the buffer to write from
 *            numbytes - the number of bytes to write
 *   RETURN VALUE: number of bytes on success, -1 otherwise.
 */
int kwrite(FILE * f, void * buf, uint32_t numbytes ){ return (*(f->optable[FILEOP_IDX_WRITE]))(f,buf,numbytes);}

/* 
 * kclose
 *   DESCRIPTION: close a file. generic function which looks up the correct operation
 *                based on filetype. Actually does nothing since none of the close
 *                operations do anything.
 *   INPUTS:  f        - the FILE to close
 *   RETURN VALUE: 0 on success (always succeeds).
 */
int kclose(FILE * f){return 0;}

/* 
 * read_dentry_by_name
 *   DESCRIPTION: read a directory entry from the filesystem by its name.
 *   INPUTS:  fname - the filename to look up and read from
 *   OUTPUTS: dentry - the dentry to read to
 *   RETURN VALUE: 0 on success, -1 if not found
 */
int32_t read_dentry_by_name (const int8_t * fname, dentry_t * dentry){
	int i;
	for (i = 0; i < zbigfs_start->num_dentry; ++i){
		if (0==strncmp(fname, zbigfs_start->dentries[i].fname, FNAME_MAX_LEN-1)){
			return read_dentry_by_index(i,dentry);
		}
	}
	return -1; //not found
}

/* 
 * read_dentry_by_index
 *   DESCRIPTION: read a directory entry from the filesystem by its index in the directory listing.
 *   INPUTS:  i - the directory index to look up and read from
 *   OUTPUTS: dentry - the dentry to read to
 *   RETURN VALUE: 0 on success, -1 if not found (the index was out of range)
 */
int32_t read_dentry_by_index (uint32_t i, dentry_t * dentry){
	if (i >= zbigfs_start->num_dentry) return -1;
	strncpy(dentry->fname, zbigfs_start->dentries[i].fname, FNAME_MAX_LEN);
	dentry->ftype = zbigfs_start->dentries[i].ftype;
	if (dentry->ftype == FTYPE_REGULAR){
		dentry->inode_num = zbigfs_start->dentries[i].inode_num;
	}
	return 0;
}

/* 
 * read_data
 *   DESCRIPTION: read a data from an inode.
 *   INPUTS:  inode - the inode to look up and read from
 *            offset - how far into the file to seek and read from
 *            length - how many bytes to read
 *   OUTPUTS: buf - the buffer to read to
 *   RETURN VALUE: number of bytes actually read on success, -1 if not found (the inode was out of range)
 */
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

/* 
 * zbigfs_mount
 *   DESCRIPTION: sets the filesystem location in memory
 *   INPUTS:  base_addr - the location in memory where the filesystem is loaded
 */
void zbigfs_mount(void* base_addr){
    zbigfs_start = base_addr;
}

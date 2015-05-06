#include "syscall_funcs.h"
#include "zbigfs.h"
#include "x86_desc.h"
#include "page.h"
#include "terminal.h"
#define MAGIC_NUM 0x464c457f
#define CHAR_NUM    32
#define LAST_INDEX	127 
#define OFFSET 24
#define EXE_HEADER_SZ (OFFSET + sizeof(int))
#define ZERO 0 
#define ONE 1
#define FAIL -1 
#define EXE_LOAD_SZ ( 33 * OFFSET_4M - LOAD_ADDR)
#define OFFSET_128M (OFFSET_4M*32)
#define OFFSET_132M (OFFSET_4M*33)

/* - - - - - - - - - - - - - - - - - - - - - - 
    Helper Functions
 - - - - - - - - - - - - - - - - - - - - - - */ 
/*
* int syscall_open(const char * name);
*   Inputs: name = name of file
*   Return Value: file descriptor or -1 on failure
*   Function: Returns a file descriptor to the file that should be opened 
*/
int syscall_open(const char * name){
    //Check validity of pointer and file descriptor
    if(! usr_ptr_ok(name, strlen(name))) return -1;
    int fd = get_new_fd();
    if (fd == -1) return -1;
    //Open the file and upon sucess return the file descriptor
    int ret = kopen(get_file(fd), name);
    if (ret){
        free_fd(fd);
        return ret;
    }
    return fd;
}

/*
* int min(int a, int b);
*   Inputs: a, first integer to compare
*           b, second integer to compare
*   Return Value: smaller number
*   Function: Compares to numbers to find minimum 
*/
int min(int a, int b){ return a<b?a:b;}

/*
* int syscall_getargs(char * buf, int nbytes);
*   Inputs: buf, buffer to write to
*           nbytes, number of bytes to copy
*   Return Value: 0 on sucess, -1 on failure
*   Function: Gets arguments from command line 
*/
int syscall_getargs(char * buf, int nbytes){
    //Check the validity of the pointer
    if(! usr_ptr_ok(buf, nbytes)) return -1;
    //Copy the command string to buffer, and if it is not all copied, return -1
    int bytes_copied = (int)strncpy(buf, get_current_pcb()->cmdstring, min(nbytes, BUF_SIZE));
    if(bytes_copied < nbytes)
        return -1;
    return 0;
}

/*
* int syscall_close(int fd);
*   Inputs: fd, a file descriptor to close it
*   Return Value: 0 on sucessful close, -1 on failure
*   Function: Closes a previously opened file
*/
int syscall_close(int fd){
    //Get a file and check if it is valid
    FILE * f = get_file(fd);
    if ((int)(f)==-1) return -1;
    //If it is close it and return based on sucess of free_fd
    kclose(f);
    return free_fd(fd);
}

/*
* void * load_exec_to_mem(const char * fnname);
*   Inputs: fname, name of file to load to memory
*   Return Value: void * of entry point into memory
*   Function: Loads executable from file into physical memory
*/
void * load_exec_to_mem( const char * fname)
{
 	FILE f;

	char exec[BUF_SIZE]; 
	char args[BUF_SIZE];
    save_hist(fname);
    //Get input fom command line
    parse_input(fname, exec, args, BUF_SIZE);
    char buf[EXE_HEADER_SZ];

    dentry_t d;
    //Read the file and check for type
    read_dentry_by_name(exec, &d);
    if (d.ftype != FTYPE_REGULAR) {return (void*) FAIL;}

    //Open the file and load it into memory
	if (kopen(&f, exec)) { return (void*)FAIL;}
    char * mem = (char *) LOAD_ADDR;
	kread(&f, buf, EXE_HEADER_SZ);

    //Check that it is an executable
    if(*(int*)buf != MAGIC_NUM) {
        return (void*)FAIL;
    }

    //Check to see if seting up a new process was sucessful
    if (setup_new_process()){
        return (void*)ONE;
    }
    //Copy it the file into memory and then create a pointer that is the entry
    // to the file's location
    memcpy(mem, buf, EXE_HEADER_SZ);
	kread(&f, mem+EXE_HEADER_SZ, EXE_LOAD_SZ);
   
    void * entry = *((void **)(mem+OFFSET));
    strncpy(get_current_pcb()->cmdstring, args, BUF_SIZE);
    return entry;
}

/*
* void * kload_exec_to_mem(const char * fnname);
*   Inputs: fname, name of file to load to memory
*   Return Value: void * of entry point into memory
*   Function: Loads executable from file into physical memory
*/
void * kload_exec_to_mem( const char * fname)
{
 	FILE f;

	char exec[BUF_SIZE]; 
	char args[BUF_SIZE];
    //Get input fom command line
    parse_input(fname, exec, args, BUF_SIZE);
    char buf[EXE_HEADER_SZ];

    dentry_t d;
    //Read the file and check for type
    read_dentry_by_name(exec, &d);
    if (d.ftype != FTYPE_REGULAR) {return (void*) FAIL;}

    //Open the file and load it into memory
	if (kopen(&f, exec)) { return (void*)FAIL;}
    char * mem = (char *) LOAD_ADDR;
	kread(&f, buf, EXE_HEADER_SZ);

    //Check that it is an executable
    if(*(int*)buf != MAGIC_NUM) {
        return (void*)FAIL;
    }

    //Check to see if seting up a new process was sucessful
    if (setup_new_process()){
        return (void*)ONE;
    }
    //Copy it the file into memory and then create a pointer that is the entry
    // to the file's location
    memcpy(mem, buf, EXE_HEADER_SZ);
	kread(&f, mem+EXE_HEADER_SZ, EXE_LOAD_SZ);
   
    void * entry = *((void **)(mem+OFFSET));
    strncpy(get_current_pcb()->cmdstring, args, BUF_SIZE);
    return entry;
}
/*
* void parse_input(const char * in, char * exec_buf, char * args_buf, int size);
*   Inputs: in, input to parse
*           exec_buf, executable buffer
*           args_buf, buffer to store args
*   Return Value: void
*   Function: Parses command line and fills buffers
*/
void parse_input(const char * in, char * exec_buf, char * args_buf, int size)
{
    int in_i = 0;
    int out_i;

    args_buf[0] = 0;

    if(in == 0)
        return;
    //Ignore leading spaces
    while(in[in_i] == ' ')
        in_i++;
    //Get first word of command into the executable name
    for(out_i = 0; in_i < size && in[in_i] != 0 && in[in_i] != '\n'; in_i++, out_i++)
    {
        if(in[in_i] == ' ')
            break;
        exec_buf[out_i] = in[in_i];
    }
    exec_buf[out_i] = '\0';
    //Ignore spaces
    while(in[in_i] == ' ')
        in_i++;
    //Get all of the arguments and put them into args buffer
    for(out_i = 0; in_i < size && in[in_i] != 0 && in[in_i] != '\n'; in_i++, out_i++)
    {
        args_buf[out_i] = in[in_i];
    }
    args_buf[out_i] = '\0';

}

/*
* int syscall_vidmap(uint8_t ** vid_ptr);
*   Inputs: vid_ptr, pointer to current video memory
*   Return Value: 0 on sucess, -1 on failure
*   Function: Maps video memory pointer 
*/
int syscall_vidmap(uint8_t ** vid_ptr)
{
    //Check validity of pointer
    if(! usr_ptr_ok(vid_ptr, sizeof(uint8_t *)))
            return -1;
    //Set video pointer to virtual memory location of video memory
    *(vid_ptr) = (uint8_t *)OFFSET_132M+OFFSET_VIDEO;
    return 0;
}

/*
* int usr_ptr_ok(const void * p, uint32_t s);
*   Inputs: p, pointer to check
*           s, size
*   Return Value: 1 on failure, 0 on sucess
*   Function: Checks validity of pointer 
*/
int usr_ptr_ok(const void * p, uint32_t s){
    //Check to make sure if pointer is inside user space
    if (((int) p) >= OFFSET_128M && ((int)p) + s <= OFFSET_132M) return 1;
    return 0;
}


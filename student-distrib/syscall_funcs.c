#include "syscall_funcs.h"
#include "zbigfs.h"
#include "x86_desc.h"
#include "page.h"
#define MAGIC_NUM 0x464c457f
#define BUFF_SIZE 128
#define CHAR_NUM    32
#define LAST_INDEX	127 
#define OFFSET 24
#define EXE_HEADER_SZ (OFFSET + sizeof(int))
#define ZERO 0 
#define ONE 1
#define FAIL -1 
#define EXE_LOAD_SZ ( 33 * OFFSET_4M - LOAD_ADDR)

/* - - - - - - - - - - - - - - - - - - - - - - 
    Helper Functions
 - - - - - - - - - - - - - - - - - - - - - - */ 

void * load_exec_to_mem( const char * fname)
{
 	FILE f;

	char arg1[BUFF_SIZE]; 
	char arg2[BUFF_SIZE];
	char arg3[BUFF_SIZE]; 

	buffer_parser((char *)&arg1, (char *)&arg2, (char *)&arg3, fname); 
    char buf[EXE_HEADER_SZ];
	if (kopen(&f, arg1)) { return (void*)FAIL;}
    char * mem = (char *) LOAD_ADDR;
	kread(&f, buf, EXE_HEADER_SZ);

    if(*(int*)buf != MAGIC_NUM) {
        return (void*)FAIL;
    }

    if (setup_new_process()){
        return (void*)ONE;
    }
    memcpy(mem, buf, EXE_HEADER_SZ);
	kread(&f, mem+EXE_HEADER_SZ, EXE_LOAD_SZ);
   
    void * entry = *((void **)(mem+OFFSET));
    return entry;
}

int buffer_parser(char * arg1, char * arg2, char * arg3, const char * s)
{
    int i, j; 
    int arg1_first = FAIL, arg1_last = FAIL; 
    int arg2_first = FAIL, arg2_last = FAIL; 
    int arg3_first = FAIL, arg3_last = FAIL;
    int flag1 = ZERO, flag2 = ZERO, flag3 = ZERO; 
    int end = FAIL; 

    /* - - - - - - - - - - - - - - - - - - - - - - 
        TEXT Buffer = empty condition 
     - - - - - - - - - - - - - - - - - - - - - - */ 
    for (i = ZERO; i < BUFF_SIZE; i++)
    {
        if ( (s[i] == '\n') || (s[i] == ZERO) ) 
        {
            end = i; 
            break;
        }
    }
    if (end == ZERO)
        return FAIL;



    /* - - - - - - - - - - - - - - - - - - - - - - 
        Finding the first term 
     - - - - - - - - - - - - - - - - - - - - - - */ 
    for (i = ZERO; i < end; i++)
    {   
        if ( (CHAR_NUM < s[i]) && (s[i] < LAST_INDEX) )
        {
            arg1_first = i; 
            flag1 = ONE; 
            break; 
        }
    }

    if (arg1_first == FAIL)
        return FAIL; 

    for (i = arg1_first+ONE; i <= end; i++)
    {
        if (!((CHAR_NUM < s[i]) && (s[i] < LAST_INDEX))) 
        {
            arg1_last = i; 
            break;
        }
    }

    if (arg1_last == FAIL)
        return FAIL; 

    j = ZERO; 
    for (i = arg1_first; i < arg1_last; i++)
    {
        arg1[j] = s[i]; 
        j++; 
    }
    arg1[j] = ZERO; 

    if (flag1){
			//Do nothing
    }else{
        arg1[ZERO] = ZERO;  
    }
    


    /* - - - - - - - - - - - - - - - - - - - - - - 
        Finding the second term 
     - - - - - - - - - - - - - - - - - - - - - - */
    for (i = arg1_last; i < end; i++)
    {   
        if ( (CHAR_NUM < s[i]) && (s[i] < LAST_INDEX) )
        {
            arg2_first = i; 
            flag2 = ONE; 
            break; 
        }
    }

    for (i = arg2_first+ONE; i <= end; i++)
    {
        if (!((CHAR_NUM < s[i]) && (s[i] < LAST_INDEX))) 
        {
            arg2_last = i; 
            break;
        }
    }

    if (arg2_last == FAIL)
        return FAIL; 

    j = ZERO; 
    for (i = arg2_first; i < arg2_last; i++)
    {
        arg2[j] = s[i]; 
        j++; 
    }
    arg2[j] = ZERO;

    if (flag2 && flag1){
        //printf("Term 2: %s\n", arg2);
    }else{
        arg2[ZERO] = ZERO;  
    }
 



    /* - - - - - - - - - - - - - - - - - - - - - - 
        Finding the third term 
     - - - - - - - - - - - - - - - - - - - - - - */
    for (i = arg2_last; i < end; i++)
    {   
        if ( (CHAR_NUM < s[i]) && (s[i] < LAST_INDEX) )
        {
            arg3_first = i; 
            flag3 = ONE; 
            break; 
        }
    }

    for (i = arg3_first+ONE; i <= end; i++)
    {
        if (!((CHAR_NUM < s[i]) && (s[i] < LAST_INDEX))) 
        {
            arg3_last = i; 
            break;
        }
    }

    if (arg3_last == FAIL)
        return FAIL; 

    j = ZERO; 
    for (i = arg3_first; i < arg3_last; i++)
    {
        arg3[j] = s[i]; 
        j++; 
    }
    arg3[j] = ZERO;

    if (flag3 && flag2 && flag1){
        //printf("Term 3: %s\n", arg3);
    }else{
        arg3[ZERO] = ZERO;  
    }
 

    /* - - - - - - - - - - - - - - - - - - - - - - 
       If All goes smoothly, return ZERO. 
     - - - - - - - - - - - - - - - - - - - - - - */
    return ZERO;
}



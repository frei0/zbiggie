#include "syscall_funcs.h"
#include "zbigfs.h"

int halt_call(){
    asm volatile(
            "movl $1, %eax;"
            "int $0x80;"
            );

    return 0;
}

int execute_call(){

    char test_buff[] = "execute shell case"; 
    char term1[128]; 
    char term2[128];
    char term3[128];
    buffer_parser(term1, term2, term3, test_buff);

    if (term1[0] != 0)
        //printf( "%s\n" , term1); 
        
    if (term2[0] != 0)
        //printf( "%s\n" , term2); 

    if (term3[0] != 0)
        //printf( "%s\n" , term3);

    asm volatile(
            "movl $2, %eax;"
            "int $0x80;"
            );

    return 0; 
}

int read_call(){

    return 0;
}

int write_call(){

    return 0;
}

int open_call(){

    return 0;
}

int close_call(){

    return 0;
}

int getargs_call(){

    return 0;
}

int vidmap_call(){

    return 0;
}

int set_handler_call(){

    return 0;
}

int sigreturn_call(){

    return 0;
}




/* - - - - - - - - - - - - - - - - - - - - - - 
    Helper Functions
 - - - - - - - - - - - - - - - - - - - - - - */ 

void * load_exec_to_mem()
{
	FILE f;
    int i;
    char * mem = 0x08048000;
    /*char buf[10000];
    for(i = 0; i<10000;i++)
        buf[i] = 0;
        */
	kopen(&f, "ls");
	kread(&f, mem, 1000);
    return mem;
}

int buffer_parser(char * arg1, char * arg2, char * arg3, char * s)
{
    int i, j; 
    int arg1_first = -1, arg1_last = -1; 
    int arg2_first = -1, arg2_last = -1; 
    int arg3_first = -1, arg3_last = -1;
    int flag1 = 0, flag2 = 0, flag3 = 0; 
    int end = -1; 

    /* - - - - - - - - - - - - - - - - - - - - - - 
        TEXT Buffer = empty condition 
     - - - - - - - - - - - - - - - - - - - - - - */ 
    for (i = 0; i < 128; i++)
    {
        if ( (s[i] == '\n') || (s[i] == 0) ) 
        {
            end = i; 
            break;
        }
    }
    if (end == 0)
        return -1;



    /* - - - - - - - - - - - - - - - - - - - - - - 
        Finding the first term 
     - - - - - - - - - - - - - - - - - - - - - - */ 
    for (i = 0; i < end; i++)
    {   
        if ( (32 < s[i]) && (s[i] < 127) )
        {
            arg1_first = i; 
            flag1 = 1; 
            break; 
        }
    }

    if (arg1_first == -1)
        return -1; 

    for (i = arg1_first+1; i <= end; i++)
    {
        if (!((32 < s[i]) && (s[i] < 127))) 
        {
            arg1_last = i; 
            break;
        }
    }

    if (arg1_last == -1)
        return -1; 

    j = 0; 
    for (i = arg1_first; i < arg1_last; i++)
    {
        arg1[j] = s[i]; 
        j++; 
    }
    arg1[j] = 0; 

    if (flag1){
        //printf("Term 1: %s\n", arg1);
    }else{
        arg1[0] = 0;  
    }
    


    /* - - - - - - - - - - - - - - - - - - - - - - 
        Finding the second term 
     - - - - - - - - - - - - - - - - - - - - - - */
    for (i = arg1_last; i < end; i++)
    {   
        if ( (32 < s[i]) && (s[i] < 127) )
        {
            arg2_first = i; 
            flag2 = 1; 
            break; 
        }
    }

    for (i = arg2_first+1; i <= end; i++)
    {
        if (!((32 < s[i]) && (s[i] < 127))) 
        {
            arg2_last = i; 
            break;
        }
    }

    if (arg2_last == -1)
        return -1; 

    j = 0; 
    for (i = arg2_first; i < arg2_last; i++)
    {
        arg2[j] = s[i]; 
        j++; 
    }
    arg2[j] = 0;

    if (flag2 && flag1){
        //printf("Term 2: %s\n", arg2);
    }else{
        arg2[0] = 0;  
    }
 



    /* - - - - - - - - - - - - - - - - - - - - - - 
        Finding the third term 
     - - - - - - - - - - - - - - - - - - - - - - */
    for (i = arg2_last; i < end; i++)
    {   
        if ( (32 < s[i]) && (s[i] < 127) )
        {
            arg3_first = i; 
            flag3 = 1; 
            break; 
        }
    }

    for (i = arg3_first+1; i <= end; i++)
    {
        if (!((32 < s[i]) && (s[i] < 127))) 
        {
            arg3_last = i; 
            break;
        }
    }

    if (arg3_last == -1)
        return -1; 

    j = 0; 
    for (i = arg3_first; i < arg3_last; i++)
    {
        arg3[j] = s[i]; 
        j++; 
    }
    arg3[j] = 0;

    if (flag3 && flag2 && flag1){
        //printf("Term 3: %s\n", arg3);
    }else{
        arg3[0] = 0;  
    }
 

    /* - - - - - - - - - - - - - - - - - - - - - - 
       If All goes smoothly, return 0. 
     - - - - - - - - - - - - - - - - - - - - - - */
    return 0;
}



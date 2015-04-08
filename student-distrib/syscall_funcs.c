#include "syscall_funcs.h"

int halt_call(){

    asm volatile(
            "movl $1, %eax;"
            "int $0x80;"
            );
    return 0;
}

int execute_call(){

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

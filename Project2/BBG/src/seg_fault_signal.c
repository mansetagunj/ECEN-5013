/**
 * @brief Credits :https://github.com/adarqui/darqbot/blob/master/test/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
 * 
 * @file seg_fault_signal.c
 * @author https://github.com/adarqui/darqbot/blob/master/test/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
 * @date 2018-04-27
 */

#include <execinfo.h>                                                                                      
#include <signal.h>                                                                                        
#include <stdio.h>                                                                                         
#include <stdlib.h>                                                                                        
#include <string.h>                                                                                        
#include <ucontext.h>   
#include <stdio.h>                                                                                 
#include <unistd.h>              
                                                                          
                                                                                                            
/* This structure mirrors the one found in /usr/include/asm/ucontext.h */                                  
typedef struct _sig_ucontext {                                                                             
unsigned long     uc_flags;                                                                               
struct ucontext   *uc_link;                                                                               
stack_t           uc_stack;                                                                               
struct sigcontext uc_mcontext;                                                                            
sigset_t          uc_sigmask;                                                                             
} sig_ucontext_t;                                                                                          
                                                                                                            
void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext)                                         
{                                                                                                          
    void *             array[50];                                                                             
    void *             caller_address = 0;                                                                        
    char **            messages;                                                                              
    int                size, i;                                                                               
    sig_ucontext_t *   uc;                                                                                    
                                                                                                                
    uc = (sig_ucontext_t *)ucontext;                                                                          
                                                                                                                
    /* Get the address at the time the signal was raised from the EIP (x86) */                                
    caller_address = (void *) uc->uc_mcontext.arm_ip;                                                            
                                                                                                                
    fprintf(stderr, "signal %d (%s), address is %p from %p\n",                                                
        sig_num, strsignal(sig_num), info->si_addr,                                                              
        (void *)caller_address);                                                                                 
                                                                                                                
    size = backtrace(array, 50);                                                                              
                                                                                                                
    /* overwrite sigaction with caller's address */                                                           
    array[1] = caller_address;                                                                                
                                                                                                                
    messages = backtrace_symbols(array, size);                                                                
                                                                                                                
    /* skip first stack frame (points here) */                                                                
    for (i = 1; i < size && messages != NULL; ++i)                                                            
    {                                                                                                         
        fprintf(stderr, "[bt]: (%d) %s\n", i, messages[i]);                                                      
    }                                                                                                         
                                                                                                                
    free(messages);                                                                                           
                                                                                                                
    //exit(EXIT_FAILURE);                                                                                       
}               


void handler(int sig) 
{
    void *array[50];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 50);
    fprintf(stderr, "Error: size %u:\n", size);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}


void install_segfault_signal()
{
     struct sigaction sigact;                                                                                  
                                                                                                                              
    sigact.sa_sigaction = crit_err_hdlr;                                                                      
    sigact.sa_flags = SA_RESTART | SA_SIGINFO;                                                                
                                                                                                                
    if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0)                                           
    {                                                                                                         
        fprintf(stderr, "error setting signal handler for %d (%s)\n",SIGSEGV, strsignal(SIGSEGV));                                                                                                                                                                                        
        exit(EXIT_FAILURE);                                                                                      
    }   

    //signal(SIGSEGV, handler);  
}
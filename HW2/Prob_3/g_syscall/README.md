# LINUX KERNEL SYSCALL IMPLEMENTATION

## SETUP STEPS FOR ADDING SYSCALL INTO KERNEL SOURCE TREE

1.  Copy this folder to the Linux kernel source tree  
2.  Add this folder path in the main Makefile of kernel available in the root of the kernel source tree  
    Old:  
    ```
    ifeq ($(KBUILD_EXTMOD),)
    core-y          += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/
    ```

    After editing:  
    ```
    ifeq ($(KBUILD_EXTMOD),)
    core-y          += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ g_syscall/
    ```
3.  Add the system call function prototype in the specific syscall table depending on your system.
    x86 with 64 bit
    
    
## BUILD THE KERNEL
Use the BuildLinuxKernel.sh script to build and install the new kernel having your syscall.

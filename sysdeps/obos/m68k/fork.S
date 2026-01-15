.section .text

#define Sys_ThreadContextCreateFork 0x80000002
#define Sys_HandleClone 0x5
#define Sys_HandleClose 0x4
#define Sys_ProcessStart 0x14
#define Sys_ThreadCreate 0x9
#define Sys_ProcessGetPid 0x3A
#define Sys_MmFork 0x3D

#define HANDLE_CURRENT 0xfe000000

#define THREAD_PRIORITY_NORMAL 2

/*
    - Set *child to zero
    - Call Sys_MmFork
    - Make a thread context for the child process
    - Create a thread for the child process
    - Create the child process
    - Get the child process PID, store it in *child
    - Cleanup (close the handles n'stuff)
*/

obos_fork:
    move.l %fp, %sp@-
    move.l %sp, %fp

    // Save registers
    move.l %d2, %sp@-
    move.l %d3, %sp@-
    move.l %d4, %sp@-
    move.l %d5, %sp@-
    // forked context=%a2
    move.l %a2, %sp@-
    // thread ctx=%a3
    move.l %a3, %sp@-
    // thread handle=%a4
    move.l %a4, %sp@-
    // process handle=%a5
    move.l %a5, %sp@-

    // Set *child to zero
    move.l (+8,%fp), %a0
    move.l #0, (%a0)

    // Fork the current VMM context
    move.l #Sys_MmFork, %d5
    trap #0
    move.l %d0, %a2

    // Make the thread context
    move.l #fork_shim, %d0
    move.l %sp, %d1
    move.l %a2, %d2
    move.l #Sys_ThreadContextCreateFork, %d5
    trap #0
    move.l %d0, %a3

    // Create a thread
    move.l #Sys_ThreadCreate, %d5
    move.l #THREAD_PRIORITY_NORMAL, %d0
    move.l #0, %d1
    move.l %a3, %d2
    trap #0
    move %d0, %a4
    
    move #Sys_ProcessStart, %d5
    move.l %a2, %d1
    move.l #1, %d2
    // d0 already has the thread handle
    trap #0
    move.l %d0, %a5

    move #Sys_ProcessGetPid, %d5
    // d0 already has the process handle
    trap #0

    // Set *child to the PID
    move.l (+8,%fp), %a0
    move.l %d0, (%a0)

    // Close the handles.

    move #Sys_HandleClose, %d5
    move %a2, %d0
    trap #0

    move #Sys_HandleClose, %d5
    move %a3, %d0
    trap #0

    move #Sys_HandleClose, %d5
    move %a4, %d0
    trap #0

    move #Sys_HandleClose, %d5
    move %a5, %d0
    trap #0

    // Restore registers
fork_shim:
    move.l %sp@+, %a5
    move.l %sp@+, %a4
    move.l %sp@+, %a3
    move.l %sp@+, %a2
    move.l %sp@+, %d5
    move.l %sp@+, %d4
    move.l %sp@+, %d3
    move.l %sp@+, %d2
    move.l %sp@+, %fp
    sub %d0, %d0
    rts

.global obos_fork
.hidden obos_fork

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()

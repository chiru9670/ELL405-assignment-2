#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int
main(int argc, char *argv[]) {

    // allocate 15 pages memory
    int i, j;
    int* ptr[18];

    for(int i=0;i<15;i++) {
        ptr[i] = (int*) malloc(PGSIZE);
        for(int j=0;j<PGSIZE/4;j++) {
            ptr[i][j] = j; 
        }
    }

    processDetailViewer();

    for(int i=15;i<18;i++) {
        ptr[i] = (int*) malloc(4096);
        
        for(int j=0;j<PGSIZE/4;j++) {
            ptr[i][j] = j; 
        }

        processDetailViewer();
    }

    for(int i=0;i<5;i++) {
        cprintf(1, "Number ptr[%d][546] = %d", i, ptr[i][546]);
    }
    processDetailViewer();

    int a = fork();

    if(a == 0) {
        cprintf(1, "Child is running, pid = %d", getpid());
        processDetailViewer();

        cprintf("Printing a random value ptr[5][100] = %d", ptr[5][100]);
        processDetailViewer();
        exit();
    }
    else {
        wait();

        sbrk(-18*PGSIZE);
        processDetailViewer();

        for(int i=0;i<5;i++) ptr[i] = (int*) malloc(PGSIZE);

        processDetailViewer();
    }

}
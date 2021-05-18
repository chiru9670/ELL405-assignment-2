#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int
main(int argc, char *argv[]) {

    // allocate 15 pages memory
    int* ptr[18];

    for(int i=0;i<15;i++) {
        ptr[i] = (int*) malloc(PGSIZE);
        for(int j=0;j<PGSIZE/4;j++) {
            ptr[i][j] = j; 
        }
    }

    processDetailsPrint();

    for(int i=15;i<18;i++) {
        ptr[i] = (int*) malloc(4096);
        
        for(int j=0;j<PGSIZE/4;j++) {
            ptr[i][j] = j; 
        }

        processDetailsPrint();
    }

    for(int i=0;i<5;i++) {
        printf(1, "Number ptr[%d][546] = %d", i, ptr[i][546]);
    }
    processDetailsPrint();

    int a = fork();

    if(a == 0) {
        printf(1, "Child is running, pid = %d", getpid());
        processDetailsPrint();

        printf(1, "Printing a random value ptr[5][100] = %d", ptr[5][100]);
        processDetailsPrint();
        exit();
    }
    else {
        wait();

        sbrk(-18*PGSIZE);
        processDetailsPrint();

        for(int i=0;i<5;i++) ptr[i] = (int*) malloc(PGSIZE);

        processDetailsPrint();
    }

}
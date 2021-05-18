#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096

int
main(int argc, char *argv[]) {

    // allocate 15 pages memory
    char* ptr[18];
    const int n = 5;
    printf(1, "\nallocate %d pages memory\n", n);
    for(int i=0;i<n;i++) {
        ptr[i] = sbrk(PGSIZE);
        for(int j=0;j<PGSIZE/sizeof(char);j+=sizeof(char)) {
            ptr[i][j] = 'j'; 
        }
    }

    processDetailsPrint();

    // printf(1, "allocating 1 more page...\n");
    // ptr[12] = sbrk(PGSIZE);
    // for (int j = 0; j < PGSIZE / sizeof(char); j += sizeof(char))
    // {
    //     ptr[12][j] = 'j';
    // }

    // processDetailsPrint();

    const int m = 8;
    printf(1, "\nallocate %d more pages\n", m);
    for(int i=n;i<n+m;i++) {
        ptr[i] = sbrk(PGSIZE);
        printf(1, "\nPage %d\n", i);
        printf(1, "\nptr[%d] = %x\n", i, (uint)ptr[i]);
        for(int j=0;j<PGSIZE/sizeof(char);j+=sizeof(char)) {
            ptr[i][j] = 'j'; 
        }

        processDetailsPrint();
    }

    // for(int i=0;i<5;i++) {
    //     printf(1, "Number ptr[%d][546] = %d", i, ptr[i][546]);
    // }
    // processDetailsPrint();

    // int a = fork();

    // if(a == 0) {
    //     printf(1, "Child is running, pid = %d", getpid());
    //     processDetailsPrint();

    //     printf(1, "Printing a random value ptr[5][100] = %d", ptr[5][100]);
    //     processDetailsPrint();
    //     exit();
    // }
    // else {
    //     wait();

    //     sbrk(-18*PGSIZE);
    //     processDetailsPrint();

    //     for(int i=0;i<5;i++) ptr[i] = (int*) sbrk(PGSIZE);

    //     processDetailsPrint();
    // }
    exit();

}
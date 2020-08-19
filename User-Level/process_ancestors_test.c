#include "process_ancestors.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <sys/syscall.h>

#define _ANCESTORS_ 342

int main(int argc, char *argv[])
{
    printf("\n--- STARTING SYSTEM CALL TESTS --- \n\n");
    long num_filled;
    srand (time(NULL));
    const long actualSize = rand()%20 + 1;
    struct process_info allProcesses[actualSize];
    printf("The actual size including swapper: %li\n\n", actualSize);

    //Performs test and displays values of processes
    printf("ENTERING KERNEL LEVEL\n");
    int result = syscall(_ANCESTORS_, allProcesses, actualSize, &num_filled);
    printf("RETURNING TO USER LEVEL\n\n");
    printf("num_filled = %li\n\n", num_filled);

    for(int i=0; i<num_filled; i++) {
	    printf("allProcesses[%d] \n", i);
	    printf("pid: %li\n", allProcesses[i].pid);
	    printf("name: %s\n", allProcesses[i].name);
	    printf("state: %li\n", allProcesses[i].state);
	    printf("uid: %li\n", allProcesses[i].uid);
	    printf("nvcsw: %li\n", allProcesses[i].nvcsw);
	    printf("nivscw: %li\n", allProcesses[i].nivcsw);
	    printf("num_children: %li\n", allProcesses[i].num_children);
	    printf("num_siblings: %li\n", allProcesses[i].num_siblings);
	    printf("\n");
    }

    //Tests for correct result and iterations
    printf("\n");
    assert(result == 0);
    assert(num_filled <= actualSize);

    //Tests invalid size value
    printf("ENTERING KERNEL LEVEL -> TESTING ARRAY SIZE\n");
    result = syscall(_ANCESTORS_, allProcesses, (long)(-2), &num_filled);
    assert(result == -1);
    assert(errno == EINVAL);
    printf("RETURNING TO USER LEVEL\n\n");

    //Tests invalid num_filled pointer
    printf("ENTERING KERNEL LEVEL -> TESTING NULL VALUE\n");
    result = syscall(_ANCESTORS_, allProcesses, actualSize, NULL);
    assert(result == -1);
    assert(errno == EFAULT);
    printf("RETURNING TO USER LEVEL\n\n");

    //Tests invalid array pointer
    printf("ENTERING KERNEL LEVEL -> TESTING INVALID POINTERS\n");
    result = syscall(_ANCESTORS_, NULL, actualSize, &num_filled);
    assert(result == -1);
    assert(errno == EFAULT);
    printf("RETURNING TO USER LEVEL\n\n");

    printf("--- TESTS FINISHED ---\n\n");

    return 0;

}

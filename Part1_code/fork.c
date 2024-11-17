#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid1, pid2;
    unsigned i;
    unsigned niterations = 100;

    //first fork: First child process
    pid1 = fork();

    if (pid1 == 0){
        //First child process
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
        printf("In first child => Own pid : %d\n", getpid());
        printf("In first child => Parent's pid : %d\n", getppid());
        return 0;
    }

    //Second fork: second child process
    pid2 = fork();

    if(pid2 == 0) {

        for(i = 0; i< niterations; ++i)
            printf("C = %d, ", i);
        printf("In second child => Own pid : %d\n", getpid());
        printf("In second child => Parent's pid : %d\n", getppid());
        return 0;
    }

    if(pid1 > 0 && pid2 > 0){
        //parent process
        for (i = 0; i < niterations; ++i)
            printf("B = %d, ", i);
        printf("\nIn parent => First child's pid: %d\n", pid1);
        printf("In parent => Second child's pid: %d\n", pid2);
        return 0;
    }

    //pid = fork();
    //if (pid == 0) {
    //    for (i = 0; i < niterations; ++i)
    //        printf("A = %d, ", i);
    //        printf("In child => Own pid : %d\n", getpid());
    //        printf("In child => Parent's pid : %d\n", pid);
    // } else {
    //    for (i = 0; i < niterations; ++i)
    //        printf("B = %d, ", i);
    //        printf("In child => Own pid : %d\n", getpid());
    //        printf("In child => Parent's pid : %d\n", pid);
    //}
    //printf("\n");
    
    // }
    // else{
    //   printf("In Parent => Child's pid is %d\n", pid);
}

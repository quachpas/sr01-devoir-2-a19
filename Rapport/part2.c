#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    for (size_t i = 1; i < 5; i++) {
        int n = fork();
        if (n > 0) {
            printf("[parent][PID=%d] Le PID de mon fils est : %d.\n", getpid(), n);
            sleep(1);
        }
        else {
            printf("[fils][PID=%d] Le PID de mon père est : %d.\n", getpid(), getppid());
        }
    }
    return EXIT_SUCCESS;
}

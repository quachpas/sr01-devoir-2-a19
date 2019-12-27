#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    for (size_t i = 1; i < 5; i++) {
        int n = fork();
        if (n > 0) {
            printf("Je suis le parent.\n Le PID de mon père est : %d.\n Le PID de mon fils est : %d.\n", getppid(), getpid());
        }
        else {
            printf("Je suis le fils.\n Le PID de mon père est : %d.\n Le PID de mon fils est : %d.\n", getppid(), getpid());
        }
    }
    return EXIT_SUCCESS;
}

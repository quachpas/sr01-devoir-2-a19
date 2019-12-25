//ApplicationManager.c
#include <signal.h>    /* sigaction... */
#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */

// Proto pour exécution des applications
// To do : lecture list_appli.txt
// intégration des données aux exécutions
#define DUREE 10
int main(int argc, char const *argv[])
{
    pid_t pid_fils;
    int i;

    switch (pid_fils = fork())
    {
    case (pid_t)-1:
        perror("fork");
        return EXIT_FAILURE;
    case (pid_t)0:
        printf("Exécution du processus fils\n");
        execl("SR01_DEVOIR_2/get_time", "", NULL);

        perror("execl");
        return EXIT_FAILURE;
    default:
        for (i = DUREE; i > 0; i--)
        {
            printf("Processus père vit encore %d secondes\n", i);
            fflush(stdout);
            sleep(1);
        }
    }
    
    return EXIT_SUCCESS;
}




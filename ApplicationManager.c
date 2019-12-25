//ApplicationManager.c
#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */
#include <string.h>
// Proto pour exécution des applications
// To do : lecture list_appli.txt
// intégration des données aux exécutions
#define DUREE 10
int main(int argc, char const *argv[])
{
    /* LECTURE LIST_APPLI.TXT */
    char buff[50];
    int nombre_app;
    int i=0; // ID du processus en cours = ID de l'app
    int n;
    int est_parent=1;
    pid_t pid_fils;
    FILE* pnt = fopen("./list_appli.txt", "r");
    if (!pnt) {
        //perror(fopen);
        return EXIT_FAILURE;
    }
    fgets(buff, 50, pnt);
    nombre_app = atoi(strrchr(buff, '=')+1); // On récupère le nombre d'app
    // printf("%d", nombre_app); // testing
    while (est_parent)
    {
        // On vérifie qu'on ne soit pas arrivé à la fin des apps
        if (i < nombre_app) {
            /* LECTURE D'UNE APP */
            while (buff[0] != '\n' && n != NULL) 
            {
                // printf("%d", i); // Testing
                printf("%s", buff);
                n = fgets(buff, 100, pnt);
            };
            i++; // On a fini de lire une app 
            /* FIN LECTURE D'UNE APP =>  FORK */
            pid_fils = fork();
            if (pid_fils > 0) {
                // Si on est le parent, on continue à lire
                fgets(buff, 100, pnt);
            }
            else {
                // Si on est le fils, on va éxécuter le programme
                est_parent = 0;
                printf("\nJe suis le fils de PID (%d). Je vais exécuter l'ap d'id %d\n", getpid(), i);
            }
        } 
        // Si on est à la fin, on s'arrête. 
        else {
           est_parent = 0;
        }
    }
    //fflush(stdout);
    fclose(pnt);
    /* EXECUTION MANAGER
    

    switch (pid_fils = fork())
    {
    case (pid_t)-1:
        perror("fork");
        return EXIT_FAILURE;
    case (pid_t)0:
        printf("Exécution du processus fils\n");
        execl("./get_time", "", NULL);

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
    */
    return EXIT_SUCCESS;
}




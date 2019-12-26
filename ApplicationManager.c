//ApplicationManager.c
#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <signal.h>     /* sigaction... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */
#include <string.h>
// Proto pour exécution des applications
// To do : lecture list_appli.txt
// intégration des données aux exécutions
#define DUREE 1000
pid_t pid;

void terminer(void)
{
	printf("\nTerminaison du processus %d", pid);
}

void quitter(void)
{
	printf("\nFin\n");
	exit(EXIT_SUCCESS);
}

void intercepter(int n)
{
    int i;

    printf("\nRéception du signal %d (INT=%d, TERM=%d, QUIT=%d)", n, SIGINT, SIGTERM, SIGQUIT);
    // (INT=2, TERM=15, QUIT=3)
	switch ( n )
		{
		case SIGTERM:
			terminer();
		case SIGINT:
		case SIGQUIT:
			quitter();
		}

	printf("\nFin du handler");
	
}



int main(int argc, char const *argv[])
{
    /* GESTION DES PROCESSUS */
    struct sigaction S;
    S.sa_handler = intercepter; 
    // La fonction qui s'occupe des sig est intercepter()

    /* LECTURE LIST_APPLI.TXT */
    char buff[50];
    char nom[50];
    char path[50];
    char nombre_arg[50];
    char args[50];
    char *ptr;
    int nombre_app;
    int i=0; // ID du processus en cours = ID de l'app
    int j,k;
    int n;
    int est_parent=1;
    pid_t pid_fils;
    FILE* pnt = fopen("./list_appli.txt", "r");
    if (!pnt) {
        //perror(fopen);
        return EXIT_FAILURE;
    }
    n = fgets(buff, 50, pnt);
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
                fgets(buff, 100, pnt);
                strcpy(nom, strrchr(buff, '=')+1);
                strrchr(nom, '\n')[0]='\0'; // On supprime le '\n'
                
                fgets(buff, 100, pnt);
                strcpy(path, strrchr(buff, '=')+1);
                strrchr(path, '\n')[0]='\0';

                fgets(buff, 100, pnt);
                strcpy(nombre_arg, strrchr(buff, '=')+1);
                strrchr(nombre_arg, '\n')[0]='\0';

                fgets(buff, 100, pnt); // suivant 
                if (atoi(nombre_arg)>0) 
                {
                    fgets(buff, 100, pnt); // Sauter 'arguments='
                    strcpy(args, buff); // Copier le premier arg                
                    strrchr(args, '\n')[0]=' ';
                    fflush(stdout); 
                    for (j=2;j<=atoi(nombre_arg);j++) {
                        fgets(buff, 100, pnt); // Suivant
                        strcpy(strrchr(args, ' ')+1, buff); // Ligne inutile du coup          
                        strrchr(args, '\n')[0]=' ';
                    }
                    strrchr(args, ' ')[0]='\0';
                }
                else {
                    args[0]='\0';
                }
                n = fgets(buff, 100, pnt); // On passe à la ligne suivante
                // C'est normalement un '\n', on sort du while ! On passe à l'app suivante.
            };
            /* FIN LECTURE D'UNE APP =>  FORK */
            pid_fils = fork();

            i++; // On a fini de lire une app, donc on incrémente le compteur.
            if (pid_fils > 0) {
                // Si on est le parent, on continue à lire
                if (i < nombre_app)
                {
                    strrchr(buff, '\n')[0] = '\0';
                }
            }
            else {
                // Si on est le fils, on va éxécuter le programme
                est_parent = 0;
                printf("\nJe suis le fils de PID (%d). Je vais exécuter l'ap d'id %d\nMon PID est %d\n", getpid(), i, getpid());
            }
        } 
        // Si on est à la fin, on s'arrête. 
        else {
           est_parent = 0;
           i++;
        }
    }
    //fflush(stdout);
    fclose(pnt);
    /* EXECUTION MANAGER */
    // Si tout se passe bien, les processus fils sont censé arriver là.
    // Le père arrive à la fin de la lecture du fichier donc.
    switch (i)
    {
    case 4: 
        // Le père !
        // id = nombre app + 1
        // Le parent reste ici :) jusqu'au sigterm, à changer plus tard
           for (k = DUREE; k > 0; k--)
           {
            printf("\n(parent)Processus père vit encore %d secondes\nMon PID est %d\n", k, getpid());
            fflush(stdout);
            sleep(1);
           }
        return EXIT_SUCCESS;
    default:
        // Sinon, traitement généralisé pour lancer les applications.
        printf("[%s] Je suis le processus de PID %d.\n", nom, getpid());
        printf("Je vais exécuter la commande suivante :\n%s %s", path, args);    
    }
    
    return EXIT_SUCCESS;
}




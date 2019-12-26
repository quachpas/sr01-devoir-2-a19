//ApplicationManager.c
#include <signal.h>     /* sigaction... */
#include <sys/wait.h>  /* wait */
#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */
#include <string.h>     /* string */

#define DUREE 100
pid_t pid, pid_fils;

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
    int i, statut_fils;

    printf("\n (PID=%d SIGCHLD=%d ) Réception du signal %d (INT=%d, TERM=%d, QUIT=%d)", pid, SIGCHLD, n, SIGINT, SIGTERM, SIGQUIT);
    // (INT=2, TERM=15, QUIT=3)
	/* Attente bloquante du fils */
	if ( waitpid(pid_fils, &statut_fils, WUNTRACED | WCONTINUED) == -1 )
		{
			perror("intercepter/waitpid");
		}
	else 
		{
			if( WIFEXITED(statut_fils) != 0 )
				{
					printf("\nFin normale du fils avec code retour %d\n\n",
								 WEXITSTATUS(statut_fils));
				}
			if( WIFSIGNALED(statut_fils) != 0 )
				{
					printf("\nFin du fils via signal %d non intercepte\n\n", WTERMSIG(statut_fils));
				}
			if( WIFSTOPPED(statut_fils) != 0 )
				{
					printf("\nProcesssus fils stoppe par signal %d\n\n", WSTOPSIG(statut_fils));
				}
			if( WIFCONTINUED(statut_fils) != 0 )
				{
					printf("\nProcesssus fils continue\n\n");
				}
		}
}



int main(int argc, char const *argv[])
{
    /* GESTION DES PROCESSUS */
    struct sigaction S;
    S.sa_handler = intercepter;

    // La fonction qui s'occupe des sig est intercepter()
    pid = getpid();
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
    int *tab_pid_fils;

    FILE* pnt = fopen("./list_appli.txt", "r");
    if (!pnt) {
        //perror(fopen);
        return EXIT_FAILURE;
    }
    n = fgets(buff, 50, pnt);
    nombre_app = atoi(strrchr(buff, '=')+1); // On récupère le nombre d'app
    tab_pid_fils = malloc(nombre_app*sizeof(int));
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
            tab_pid_fils[i]=pid_fils;
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
                //printf("\nJe suis le fils de PID (%d). Je vais exécuter l'ap d'id %d\nMon PID est %d\n", getpid(), i, getpid());
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
    
    
    switch (pid_fils)
    {
    case (pid_t)-1: 
        perror("main/fork");
        return EXIT_FAILURE;

    case (pid_t)0:
        // Sinon, traitement généralisé pour lancer les applications.
        printf("[%s][%d]\n", nom, getpid());
        printf("Je vais exécuter la commande suivante :\n%s %s\n\n", path, args);    
        execl(path, args);

        return EXIT_SUCCESS;
    default:
        // Le père !
        // id = nombre app + 1
        // Le parent reste ici :) jusqu'au sigterm, à changer plus tard
        sleep(4); // Délai initial
        for (k = DUREE; k > 0; k=k-10)
        {
            printf("\n\n(parent)Processus père vit encore %d secondes\nMon PID est %d\n\n", k, getpid());
            printf("Mes fils sont : ");
            for (size_t l = 0; l<nombre_app; l++)
            {
                printf("%d ", tab_pid_fils[l]);
                if( sigaction(SIGCHLD, &S, NULL) != 0 )
                {
                    perror("sigaction");
                }
            }
            printf("\n");
            fflush(stdout); 
            sleep(10);
        }        
    
    }
    
    return EXIT_SUCCESS;
}




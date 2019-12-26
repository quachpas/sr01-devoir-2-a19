/* COMMENTAIRES SUR LECTURE FICHIER 
Vu qu'on a commencé à fork() en lisant les informations, 
on ne sait pas vraiment à quel moment un des processus
enfants va mourir ...
Ce qui est très embêtant ! On ne veut pas de zombi
pour pouvoir récupérer le signal. Par manque de temps,
on se contentera donc d'ajouter des sigaction un peu partout 
en espérant que le processus parent sera en train d'attendre
le SIGCHLD au moment où les deux premiers processus enfants meurent.
Ce qu'il faudrait faire : 
- Réécrire la lecture du fichier list_app.txt pour ne 
pas implémenter la création des processus fils en parallèle.
Il faudrait donc PREMIEREMENT stocker toutes les informations 
dans un tableau du style char **nom, etc. en estimant que chaque
app a bien un nom, un path etc. 
PUIS, on irait créer les processus fils à l'aide d'une boucle style
for (size_t i = 0; i<nombre_app; i++)
{
    who_am_i=fork();
    if(who_am_i == 0)
    {
        // On sort de la boucle
        id_processus = i;
        i = nombre_app;
    }
}
On crée ainsi exactement nombre_app processus. 
Le id_processus servira à accéder aux infos qui correspondent bien
au processus. Du style, nom = *nom[id_processus];
*/
//ApplicationManager.c
#include <signal.h>     /* sigaction... */
#include <sys/wait.h>  /* wait */
#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */
#include <string.h>     /* string */

#define CODE_RETOUR_FILS 4
#define DUREE 10
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
					printf("\nFin normale du fils (%d) avec code retour %d\n\n", pid_fils, WEXITSTATUS(statut_fils));
				}
			if( WIFSIGNALED(statut_fils) != 0 )
				{
					printf("\nFin du fils (%d) via signal %d non intercepte\n\n", pid_fils, WTERMSIG(statut_fils));
				}
			if( WIFSTOPPED(statut_fils) != 0 )
				{
					printf("\nProcesssus fils (%d) stoppe par signal %d\n\n", pid_fils, WSTOPSIG(statut_fils));
				}
			if( WIFCONTINUED(statut_fils) != 0 )
				{
					printf("\nProcesssus fils (%d) continue\n\n", pid_fils);
				}
		}
}

void no_zombi(struct sigaction S) 
{
    if( sigaction(SIGCHLD, &S, NULL) != 0 )
    {
        perror("sigaction");
    }
}

int double_pointer_malloc(char **tab, int taille) 
{
    for (size_t i = 0; i < taille; i++)
    {
        tab[i] = malloc(50*sizeof(char));
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
    char buff[50]; // Buffer
    char **nom;
    char **path;
    char **nombre_arg;
    char **args;
    int who_am_i;
    char *ptr;
    int nombre_app;
    int id_processus;
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
    /* INITIALISATION DES TABLEAUX */
    tab_pid_fils = malloc(nombre_app*sizeof(int));
    nom = malloc(nombre_app*sizeof(char));
    double_pointer_malloc(nom, nombre_app);
    path = malloc(nombre_app*50*sizeof(char));
    double_pointer_malloc(path, nombre_app);
    nombre_arg = malloc(nombre_app*50*sizeof(char));
    double_pointer_malloc(nombre_arg, nombre_app);
    args = malloc(nombre_app*50*sizeof(char));
    double_pointer_malloc(args, nombre_app);
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
                strcpy(nom[i], strrchr(buff, '=')+1);
                strrchr(nom[i], '\n')[0]='\0'; // On supprime le '\n'

                fgets(buff, 100, pnt);
                strcpy(path[i], strrchr(buff, '=')+1);
                strrchr(path[i], '\n')[0]='\0';

                fgets(buff, 100, pnt);
                strcpy(nombre_arg[i], strrchr(buff, '=')+1);
                strrchr(nombre_arg[i], '\n')[0]='\0';

                fgets(buff, 100, pnt); // suivant 
                if (atoi(nombre_arg[i])>0) 
                {
                    fgets(buff, 100, pnt); // Sauter 'arguments='
                    strcpy(args[i], buff); // Copier le premier arg                
                    strrchr(args[i], '\n')[0]=' ';
                    fflush(stdout); 
                    for (j=2;j<=atoi(nombre_arg[i]);j++) {
                        fgets(buff, 100, pnt); // Suivant
                        strcpy(strrchr(args[i], ' ')+1, buff); // Ligne inutile du coup          
                        strrchr(args[i], '\n')[0]=' ';
                    }
                    strrchr(args[i], ' ')[0]='\0';
                }
                else {
                    args[i][0]='\0';
                }
                
                n = (int)fgets(buff, 100, pnt); // On passe à la ligne suivante
                // C'est normalement un '\n', on sort du while ! On passe à l'app suivante.
            }
            i++; // On a fini de lire une app, donc on incrémente le compteur.
            if (i < nombre_app)
            {
                strrchr(buff, '\n')[0] = '\0';
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
    for (size_t l = 0; l < nombre_app; l++)
    {
        pid_fils=fork();
        tab_pid_fils[l]=pid_fils;
        if(pid_fils == 0)
        {
            // On sort de la boucle
            id_processus = l;
            l = nombre_app;
        }
    }

    switch (pid_fils)
    {
    case (pid_t)-1: 
        perror("main/fork");
        return EXIT_FAILURE;

    case (pid_t)0:
        // Sinon, traitement généralisé pour lancer les applications.
        printf("[id_proc=%d][i=%d]\n", id_processus, i);
        printf("[%s][%d]\n", nom[id_processus], getpid());
        printf("Je vais exécuter la commande suivante :\n%s %s\n\n", path[id_processus], args[id_processus]);    
        execl(path[id_processus], args[id_processus]);
        return CODE_RETOUR_FILS;
    default:
        // Le père !
        // id = nombre app + 1
        // Le parent reste ici :) jusqu'au sigterm, à changer plus tard
        
        
        sleep(4); // Délai initial
        for (k = DUREE; k > 0; k=k-5)
        {
            if( sigaction(SIGCHLD, &S, NULL) != 0 )
            {
                perror("sigaction");
            }
            printf("\n\n(parent)Processus père vit encore %d secondes\nMon PID est %d\n\n", k, getpid());
            printf("Mes fils sont : ");
            for (size_t l = 0; l < nombre_app; l++)
            {
                printf("%d ", tab_pid_fils[l]);
            }
            printf("\n");
               
            fflush(stdout); 
            sleep(5);
        }     
        // Tuer tous les fils   
        for (size_t l = 0; l < nombre_app; l++)
        {
            kill(tab_pid_fils[l], SIGTERM);
        }
        
    
    }
    
    return EXIT_SUCCESS;
}




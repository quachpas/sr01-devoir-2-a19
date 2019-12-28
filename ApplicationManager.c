
//ApplicationManager.c
#include <unistd.h>     /* fork()... */
#include <signal.h>     /* sigaction... */
#include <sys/wait.h>  /* wait */
#include <sys/types.h>  /* Types pid_t... */

#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */
#include <string.h>     /* string */

#define CODE_RETOUR_FILS 4
#define DUREE 10

pid_t pid, pid_fils;
int fd[2];

void intercepter(int n)
{
    int i, statut_fils;
    char name[50];
    read(fd[0], name, 50*sizeof(char));    
    printf("\n [%s] (PID=%d SIGCHLD=%d ) Réception du signal %d\n\n", name, pid, SIGCHLD, n);
    // (INT=2, TERM=15, QUIT=3)
	
    
	if ( waitpid(pid_fils, statut_fils, WNOHANG | WCONTINUED) == -1 )
    {
        printf("<------------- %s ------------>\n ", name);
        perror("intercepter/waitpid");  
        printf("\n");
    }
	else 
    {
        printf("<----- [%s] PID=%d ------->\n", name, pid_fils);

        printf("<----- WIFEXITED=%d -------->\n", WIFEXITED(statut_fils));
        if( WIFEXITED(statut_fils) != 0 )
        {
            printf("\n[%s] Fin normale du fils (%d) avec code retour %d\n\n", name, pid_fils, WEXITSTATUS(statut_fils));
        }

        printf("<----- WIFSIGNALED=%d ------>\n", WIFSIGNALED(statut_fils));
        if( WIFSIGNALED(statut_fils) != 0 )
        {
            printf("\n[%s] Fin du fils (%d) via signal %d non intercepte\n\n", name, pid_fils, WTERMSIG(statut_fils));
        }

        printf("<------ WIFSTOPPED=%d ------>\n", WIFSTOPPED(statut_fils));
        if( WIFSTOPPED(statut_fils) != 0 )
        {
            printf("\n[%s] Processus fils (%d) stoppe par signal %d\n\n", name, pid_fils, WSTOPSIG(statut_fils));
        }

        printf("<----- WIFCONTINUED=%d ----->\n", WIFCONTINUED(statut_fils));
        if( WIFCONTINUED(statut_fils) != 0 )
        {
            printf("\n[%s] Processus fils (%d) continue\n\n", name, pid_fils);
        }
    }
}

int double_pointer_malloc(char **tab, int taille) 
{
    for (size_t i = 0; i < taille; i++)
    {
        tab[i] = malloc(50*sizeof(char));
    }
    
}

void double_pointer_free(char **tab, int taille)
{
    for (size_t i = 0; i < taille; i++)
    {
        free(tab[i]);
    }
}


int main(int argc, char const *argv[])
{
    /* GESTION DES PROCESSUS */
    struct sigaction S;
    S.sa_handler = intercepter;
    sigemptyset(&S.sa_mask);
    S.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
    if( sigaction(SIGCHLD, &S, NULL) != 0 )
    {
        perror("sigaction");
    }

    // La fonction qui s'occupe des sig est intercepter()
    pid = getpid();
    /* LECTURE LIST_APPLI.TXT */
    char buff[50]; // Buffer
    char **nom;
    char **path;
    char s_path[50];
    char **nombre_arg;
    char **args;
    char ***s_args_v;

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
    s_args_v = malloc(sizeof(char*)*nombre_app);

    
    nom = malloc(nombre_app*sizeof(char*));
    double_pointer_malloc(nom, nombre_app);
    path = malloc(nombre_app*sizeof(char*));
    double_pointer_malloc(path, nombre_app);
    nombre_arg = malloc(nombre_app*sizeof(char*));
    double_pointer_malloc(nombre_arg, nombre_app);
    args = malloc(nombre_app*sizeof(char*));
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
        pipe(fd);
        pid_fils=fork();
        tab_pid_fils[l]=pid_fils;
        if(pid_fils == 0)
        {
            // FILS
            close(fd[0]); // Fermer lecture pour le fils
            // On sort de la boucle
            id_processus = l;
            ////printf("<------ [PID=%d] ID_PROCESSUS=%d ------>\n", getpid(), id_processus);
            l = nombre_app;
        }
        else {
            // PERE 
            ////printf("<------ PID_FILS=%d ------>\n", pid_fils);
            //sleep(2);
            close(fd[1]); // Fermer l'écriture pour le père
        }
        
    }

    switch (pid_fils)
    {
    case (pid_t)-1: 
        perror("main/fork");
        return EXIT_FAILURE;

    case (pid_t)0:
        // Sinon, traitement généralisé pour lancer les applications.
        //sleep(2);
        //printf("<-------------- %s ------------>\n", nom[id_processus]);
        printf("[id_proc=%d][%s][%d]\n", id_processus, nom[id_processus], getpid());
        printf("Je vais exécuter la commande suivante :\n%s %s\n\n", path[id_processus], args[id_processus]);    
        
        strcpy(s_path, path[id_processus]); // path

        // argv
        //printf("<---------- [ID=%d] malloc s_args_v[%d] -------->\n", id_processus, id_processus);
        s_args_v[id_processus] = malloc(sizeof(char*)*atoi(nombre_arg[id_processus])+1+1); // path + nombre_arg + ending null pointer = nombre_arg +1 +1
        //printf("<---------- [ID=%d] malloc2 s_args_v[%d][0,...,%d] -------->\n", id_processus, id_processus, atoi(nombre_arg[id_processus])+1);
        double_pointer_malloc(s_args_v[id_processus], atoi(nombre_arg[id_processus])+1+1);
        if(id_processus == 0)
        {
            //printf("<---------- [ID=%d] malloc2 check : s_args_v[0][2] exists := %d -------->\n", id_processus, s_args_v[0][2]);
        }
        //printf("<---------- [ID=%d] Copying path -------->\n", id_processus);
        strcpy(s_args_v[id_processus][0], path[id_processus]);// argv[0] = path
        //printf("<-------- [ID=%d] s_args_v[%d][0] = %s -------->\n", id_processus, id_processus, s_args_v[id_processus][0]);
        //printf("<---------- [ID=%d] Copying args -------->\n", id_processus);
        
        s_args_v[id_processus][atoi(nombre_arg[id_processus])+1] = '\0'; // NULL POINTER
        //printf("<---------- [ID=%d] NULL POINTER CHECK -------->\n", id_processus);
        //printf("<---------- [ID=%d] s_args_v[%d][%d] = _%s_ -------->\n", id_processus, id_processus, atoi(nombre_arg[id_processus])+1, s_args_v[id_processus][atoi(nombre_arg[id_processus])+1]);

        switch (atoi(nombre_arg[id_processus])) 
        {
            case 0:
                //printf("<-------- [ID=%d][case=0] nombre arg = %d -------->\n", id_processus, atoi(nombre_arg[id_processus]));
                //printf("<---------- [ID=%d] NULL POINTER CHECK -------->\n", id_processus);
                //printf("<-------- [ID=%d][case=0] s_args_v[%d][1] = %s -------->\n", id_processus, id_processus, s_args_v[id_processus][1]);
                break;
            case 1:
                //printf("<-------- [ID=%d][case=1] nombre arg = %d -------->\n", id_processus, atoi(nombre_arg[id_processus]));
                strcpy(s_args_v[id_processus][1], args[id_processus]);
                //printf("<---------- [ID=%d] NULL POINTER CHECK -------->\n", id_processus);
                //printf("<-------- [ID=%d][case=1] s_args_v[%d][2] = %s -------->\n", id_processus, id_processus, s_args_v[id_processus][2]);
                break;
            default:
                //printf("<-------- [ID=%d][case=2] nombre arg = %d -------->\n", id_processus, atoi(nombre_arg[id_processus]));
                
                for (size_t k = atoi(nombre_arg[id_processus]); k >= 2; k--)
                {
                    //printf("<---------- [ID=%d][iteration=%d] Copying args -------->\n", id_processus, k);
                    if (strrchr(args[id_processus], ' ')+1) 
                    {
                        //printf("<---------- [ID=%d][iteration=%d] Copying args : check, args_[id_processus]+1 exists -------->\n", id_processus, k);
                        //printf("<---------- [ID=%d][iteration=%d] args_[id_processus]+1 =  _%s_ -------->\n", id_processus, k, strrchr(args[id_processus], ' ')+1);
                    }
                    //printf("<---------- [ID=%d][iteration=%d] s_args_v[%d][%d] size check : _%d_-------->\n", id_processus, k, id_processus, k, sizeof(s_args_v[id_processus][k]));
                    //printf("<---------- [ID=%d][iteration=%d] size check : is s_args_v[%d][%d] >= %s. BOOL = _%d_  -------->\n", id_processus, k, id_processus, k, strrchr(args[id_processus], ' ')+1, sizeof(strrchr(args[id_processus], ' ')+1) <= sizeof(s_args_v[id_processus][k]));
                    //printf("<---------- [ID=%d][iteration=%d] Creating pointer to last arg in args[] -------->\n", id_processus, k);
                    char* pnt_source = strrchr(args[id_processus], ' ')+1;
                    if (pnt_source)
                    {
                        //printf("<---------- [ID=%d][iteration=%d] Pointer successfully created. Points to : _%s_ -------->\n", id_processus, k, pnt_source);
                    }
                    else
                    {
                        //printf("<---------- [ID=%d][iteration=%d] You're a failure of a pointer damn it -------->\n", id_processus, k);
                    }
                    //printf("<---------- [ID=%d][iteration=%d] Dest array existence check := _%d_ -------->\n", id_processus, k, s_args_v[id_processus][k]);
                    //printf("<---------- [ID=%d][iteration=%d] Dest array access check ... -------->\n", id_processus, k);
                    char * pnt_dest = s_args_v[id_processus][k];
                    if (pnt_dest)
                    {
                        //printf("<---------- [ID=%d][iteration=%d] Pointer successfully created. Points to : _%s_ -------->\n", id_processus, k, pnt_dest);
                    }
                    else
                    {
                        //printf("<---------- [ID=%d][iteration=%d] You're a failure of a pointer damn it -------->\n", id_processus, k);
                    }
                    strcpy(pnt_dest, pnt_source);
                    strrchr(args[id_processus], ' ')[0] = '\0';
                    //printf("<---------- [ID=%d][iteration=%d] s_args_v[%d][%d] = %s -------->\n", id_processus, k, id_processus, k, s_args_v[id_processus][k]);
                }
                //printf("<---------- [ID=%d] existence check s_args_v[%d][%d] := _%s_ -------->\n", id_processus, id_processus, atoi(nombre_arg[id_processus])+2, s_args_v[id_processus][atoi(nombre_arg[id_processus])+1]);
                strcpy(s_args_v[id_processus][1], args[id_processus]);
                break;
        }
        write(fd[1], nom[id_processus], strlen(nom[id_processus])+1); // On passe le nom de l'app à intercepter
        /*printf("<------------ [ID=%d] s_args_v = ", id_processus);
        for (size_t k = 0; k <= atoi(nombre_arg[id_processus])+1; k++)
        {
            printf("_%s_ ", s_args_v[id_processus][k]);
        }
        printf("----------->\n");
        */
        if (execv(s_path, s_args_v[id_processus]) == - 1) // ERREUR
        {
                perror("main/execv");
                fflush(stdout);
                exit(0);
                return EXIT_FAILURE;
        }

        return CODE_RETOUR_FILS;
    default: ;
        // Le père !
        // id = nombre app + 1
        // Le parent reste ici :) jusqu'au sigterm, à changer plus tard
        
        ////printf("<-------------- %s ------------>\n", nom[id_processus]);
        //write(fd[1], nom[id_processus], strlen(nom[id_processus])+1);           
        
        int k = 0;
        unsigned int pas = 20;
        while(1)
        {
            printf("\n\n[parent][PID=%d]Processus père vit depuis %d secondes\n", getpid(), k);
            printf("Mes fils sont : ");
            k=k+pas;
            for (size_t l = 0; l < nombre_app; l++)
            {
                printf("%d ", tab_pid_fils[l]);
            }
            
            printf("\n");
            
            sleep(20);
            // Tuer tous les fils   
            
            
        }   
        for (size_t l = 0; l < nombre_app; l++)
        {
            printf("<------------ KILL(%d, SIGTERM) ---------->\n", tab_pid_fils[l]);
            kill(tab_pid_fils[l], SIGTERM);
            //sleep(1);
        }   
    }
    free(tab_pid_fils);
    double_pointer_free(nom, nombre_app);
    free(nom);
    double_pointer_free(path, nombre_app);
    free(path); 
    double_pointer_free(nombre_arg, nombre_app);
    free(nombre_arg);
    double_pointer_free(args, nombre_app);
    free(args);

    return EXIT_SUCCESS;
}

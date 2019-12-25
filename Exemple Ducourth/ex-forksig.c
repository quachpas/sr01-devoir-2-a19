#include <signal.h>    /* sigaction... */
#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */

#define CODE_RETOUR_FILS 4

pid_t pid, pid_fils;

void intercepter(int n)
{
  printf("\nProc %d : réception du signal %d (SIGCLD=%d)\n\n", pid, n, SIGCLD);
}


int main(int argc, char **argv)
{
	int i, delais_fils, delais_pere;
	struct sigaction S;

	if(argc < 3)
		{
			printf("\n Usage : %s delais_fils delais_pere\n", argv[0]);
			return EXIT_SUCCESS;
		}

	delais_fils = atoi(argv[1]);
	delais_pere = atoi(argv[2]);
	S.sa_handler = intercepter;
	pid = getpid();
	
	if( sigaction(SIGCHLD, &S, NULL) != 0 )
		{
			perror("sigaction");
			exit(EXIT_FAILURE);
		}
	
	switch ( pid_fils = fork() )
		{
		case (pid_t)-1:
			/* Retour du fork() = -1 => echec (eg. manque de ressources) */
			perror("main/fork");
			return EXIT_FAILURE;

		case (pid_t)0:
			/* Retour du fork() = 0 => processus fils */
			pid = getpid();
			for(i=delais_fils; i>0; i=i-2)
				{
					printf("\n  Proc %d (fils) : vivant pour encore %d secondes.", pid, i);
					fflush(stdout);		
					sleep(2);
				}
			return CODE_RETOUR_FILS;

		default:
			/* Retour du fork() != 0 donc c'est le pere */
			for(i=delais_pere; i>0; i=i-2)
				{
					printf("\nProc %d (pere) : vivant pour encore %d secondes.", pid, i);
					fflush(stdout);
					sleep(2);
				}
		}

	printf("\n");
	return EXIT_SUCCESS;
}


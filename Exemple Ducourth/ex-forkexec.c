#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */

#define DURATION 15

int main(int argc, char **argv)
{
	pid_t pid_fils;
	int i;
	
	if(argc < 2)
		{
			printf("\n Usage : %s commande (eg. /bin/cat)\n", argv[0]);
			return EXIT_SUCCESS;
		}

	switch ( pid_fils = fork() )
		{
		case (pid_t)-1:
			/* Retour du fork() = -1 => echec (eg. manque de ressources) */
			perror("main/fork");
			return EXIT_FAILURE;

		case (pid_t)0:
			/* Retour du fork() = 0 => processus fils */

			printf("\n   Processus fils %d de pere %d", getpid(), getppid());
			printf("\n  --- Execution de la commande %s ---\n", argv[1]);
	
			execl(argv[1], argv[1], NULL);

			perror("main/execl");
			return EXIT_FAILURE;

		default:
			/* Retour du fork() != 0 donc c'est le pere */
			for(i=DURATION; i>0; i--)
				{
					printf("\n Processus pere %d ayant cree le fils %d, vivant pour encore %d secondes.", getpid(), pid_fils, i);
					fflush(stdout);
					sleep(1);
				}
		}

	return EXIT_SUCCESS;
}


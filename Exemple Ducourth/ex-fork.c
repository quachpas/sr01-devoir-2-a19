#include <sys/types.h>  /* Types pid_t... */
#include <unistd.h>     /* fork()... */
#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */

#define DURATION 15

int main()
{
	pid_t pid_fils;
	int i;
	
	switch ( pid_fils = fork() )
		{
		case (pid_t)-1:
			/* Retour du fork() = -1 => echec (eg. manque de ressources) */
			perror("main/fork");
			return EXIT_FAILURE;

		case (pid_t)0:
			/* Retour du fork() = 0 => processus fils */
			for(i=DURATION; i>0; i--)
				{
					printf("  Le processus fils est vivant pour encore %d secondes.\n", i);
					fflush(stdout);		
					sleep(1);
				}
			/* Toujours empecher que le fils poursuive l'execution dans le default */
			return EXIT_SUCCESS;

		default:
			/* Retour du fork() != 0 donc c'est le pere */
			for(i=DURATION; i>0; i--)
				{
					printf("Le processus pere est vivant pour encore %d secondes.\n", i);
					fflush(stdout);
					sleep(1);
				}
		}

	return EXIT_SUCCESS;
}


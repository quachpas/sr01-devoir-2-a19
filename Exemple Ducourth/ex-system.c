#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE, system... */
#include <unistd.h>     /* execl... */

int main(int argc, char **argv)
{
	int i, res;
	
	if(argc < 2)
		{
			printf("\n Usage : %s commande (eg. /bin/cat)\n", argv[0]);
			return EXIT_SUCCESS;
		}

	printf("\n Processus %d", getpid());
	printf("\n--- Execution de la commande %s ---\n", argv[1]);
	if ( system(argv[1]) == -1 )
		{
			perror("main/system");
			return(EXIT_FAILURE);
		}

	printf("\n--- Fin ---\n");
	
	return(EXIT_SUCCESS);
}

			

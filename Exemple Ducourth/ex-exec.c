#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE... */
#include <unistd.h>     /* execl... */

int main(int argc, char **argv)
{
	if(argc < 2)
		{
			printf("\n Usage : %s commande (eg. /bin/cat)\n", argv[0]);
			return EXIT_SUCCESS;
		}
	
	printf("\n Processus %d", getpid());
	printf("\n--- Execution de la commande %s ---\n", argv[1]);
	
	execl(argv[1], argv[1], NULL);

	perror("main/execl");
	return(EXIT_FAILURE);
}

			

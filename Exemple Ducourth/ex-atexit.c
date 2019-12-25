#include <stdio.h>      /* printf... */
#include <stdlib.h>     /* EXIT_FAILURE, atexit... */
#include <sys/types.h>  /* getpid... */
#include <unistd.h>

pid_t pid;

void terminer(void)
{
	printf("\nTerminaison du processus %d (fonction \"terminer\")", pid);
}

void quitter(void)
{
	printf("\nFin (fonction \"quitter\")\n");
}


int main(int argc, char **argv)
{
	int i;

	if(argc < 2)
		{
			printf("\n Usage : %s delais\n", argv[0]);
			return EXIT_SUCCESS;
		}

	pid = getpid();
	
	if( atexit(quitter) != 0 || atexit(terminer) != 0)
		{
			perror("main/atexit");
			return EXIT_FAILURE;
		}
	
	for(i=atoi(argv[1]); i>0; i--)
		{
			printf("\nLe processus %d est vivant pour encore %d secondes.", pid, i);
						
			fflush(stdout);
			sleep(1);
		}
	
	return EXIT_SUCCESS;
}


#include <signal.h>    /* sigaction... */
#include <stdio.h>     /* printf... */
#include <stdlib.h>    /* EXIT_FAILURE... */
#include <sys/types.h> /* Types pid_t... */
#include <unistd.h>


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


int main(int argc, char **argv)
{
	int i;
	struct sigaction S;

	if(argc < 2)
		{
			printf("\n Usage : %s delais\n", argv[0]);
			return EXIT_SUCCESS;
		}

	pid = getpid();
	S.sa_handler = intercepter;

	/* if( sigfillset( &S.sa_mask ) != 0 ) */
	/* 	{ */
	/* 		perror("main/sigfillset"); */
	/* 		exit(EXIT_FAILURE); */
	/* 	} */
	
	/* S.sa_flags = 0; */

	if( sigaction(SIGTERM, &S, NULL) != 0 || sigaction(SIGQUIT, &S, NULL) != 0 || sigaction(SIGINT, &S, NULL) != 0)
		{
			perror("sigaction");
			exit(EXIT_FAILURE);
		}
	
	for(i=atoi(argv[1]); i>0; i--)
		{
			printf("\nLe processus %d est vivant pour encore %d secondes.", pid, i);
						
			fflush(stdout);
			sleep(1);
		}

  return EXIT_SUCCESS;
}



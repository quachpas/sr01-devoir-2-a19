#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv)
{
	int i, delais_fils, delais_pere;
	pid_t pid_fils;
	char commande[128];

	if(argc < 3)
		{
			printf("\n Usage : %s delais_fils delais_pere\n", argv[0]);
			return EXIT_SUCCESS;
		}

	delais_fils = atoi(argv[1]);
	delais_pere = atoi(argv[2]);

	switch ( pid_fils = fork() )
		{
		case (pid_t)-1:
			perror("main/fork");
			exit(EXIT_FAILURE);
		
		case (pid_t)0:
			/* processus fils */
			for(i=delais_fils; i>0; i=i-2)
				{
					printf("---> Le processus fils est vivant pour encore %d secondes.\n", i);
					fflush(stdout);		
					sleep(1);
				}
			exit(EXIT_SUCCESS);
		

		default:
			/* processus pere */
			snprintf(commande, 128, "ps %d", pid_fils);
			for(i=delais_pere; i>0; i=i-2)
				{
					printf("Le processus pere est vivant pour encore %d secondes.\n", i);
					system(commande);
					fflush(stdout);
					sleep(1);
				}
		}

	return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void main (int argc , char *argv[]) {

 FILE * fp;
 char c;
 pid_t ppid = getppid();
 while(1) 
 {
 	fp = fopen (argv[1], "r");
	if(fp == NULL) exit(EXIT_FAILURE);
	c = fgetc(fp);
	fclose(fp);
	printf("<--- fp closed ? := %d ---->\n", ftell(fp));
	printf("<- %c ->\n",c);
	printf("<--- PPID = %d ---->\n", ppid);
	if(c =='1') 
	{
		printf("[power manager] mise en veille en cours ...\n");
        /* ajoutez vos modification ici */
		
		kill(ppid, SIGUSR1);
		
		/* ajoutez vos modification ici */
	  	fp = fopen (argv[1], "w"); 
	  	fputs("0", fp);
   	  	fclose(fp);
	}
	printf("[power_manager]Je vais dormir pendant %d secondes.\n", atoi(argv[2]));
	sleep(atoi(argv[2]));
 }
}

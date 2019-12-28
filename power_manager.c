#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main (int argc , char *argv[]) {

 FILE * fp;
 char c;
 while(1) 
 {
	if(argc != 3) {
		printf("Usages : ./power_manager ./mise_en_veille.txt délai_en_secondes");
		exit(EXIT_FAILURE);
	}
 	fp = fopen (argv[1], "r");
	if(fp == NULL) exit(EXIT_FAILURE);
	c = fgetc(fp);
	fclose(fp);
	if(c =='1') {
         printf("[power manager] mise en veille en cours ...\n");
         /* ajoutez vos modification ici */
	  fp = fopen (argv[1], "w"); 
	  fputs("0", fp);
   	  fclose(fp);
	}
	printf("[power_manager]Je vais dormir pendant %d secondes.\n", atoi(argv[2]));
	sleep(atoi(argv[2]));
 }
}

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(){
  int choix,n,i,chiffre,j,k;
  char CHAINE[25];
  strcpy(CHAINE,"matrice1");
  srand(time(NULL));
  FILE * f;
  
  do{
    printf("Combien de lignes\n");
    scanf(" %d",&n)==0;
  }while(n<0);
  
  for(k=0;k<2;k++){//saisie des deux matrices
  do{
      printf("Tapez 1 pour saisir une matrice, 2 pour la générer aléatoirement\n");
      scanf(" %d",&choix);
      switch(choix){
        case 1:
    
          f=fopen(CHAINE,"wb");
          
          if(f==NULL){
            printf("Erreur lors de la creation du fichier %d\n",k);
            return 0;
          }
          for(i=0;i<n;i++){
            for (j = 0; j < n; j++) {
              scanf(" %d",&chiffre);
              fwrite(&chiffre,sizeof(int),1,f);//saisie de tous les chiffre de la matrice
            }
          }
          fclose(f);
          break;
          
        case 2:
          f=fopen(CHAINE,"wb");
          if(f==NULL){
            printf("Erreur lors de la creation du fichier %d\n",k);
            return 0;
          }
          for(i=0;i<n;i++){
            for (j = 0; j < n; j++) {
              chiffre=rand()%200;//generation aleatoire de tous les chiffres de la matrice
              fwrite(&chiffre,sizeof(int),1,f);
            }
          }
          fclose(f);
          break;
    
    
      }
    }while(choix<1||choix>2);//verification des choix
      strcpy(CHAINE,"matrice2");//pour creer le fichier matrice 2

}
//mise en place des arguments
char** tabarg,buffer[25];
tabarg=malloc(5*sizeof(char*));
for ( i = 0; i < 5; i++) {
  tabarg[i]=malloc(25*sizeof(char));
}
strcpy(tabarg[1],"matrice1");
strcpy(tabarg[2],"matrice2");
sprintf(buffer,"%d",n);
strcpy(tabarg[3],buffer);
tabarg[4]=NULL;

do{
printf("Que voulez vous faire?\n1:Addition\n2:Multiplication\n");
scanf(" %d",&choix);

switch(choix){
  case 2:
    printf("Appel de Produit\n" );
    strcpy(tabarg[0],"./Produit");
    execv("./Produit",tabarg);

    break;
  case 1:
    printf("Appel de Somme\n" );
    strcpy(tabarg[0],"./Somme");
    execv("./Somme",tabarg);
    break;


  }
}while(choix<1||choix>2);
for ( i = 0; i < 4; i++) {
  free(tabarg[i]);
}
free(tabarg);
return 0;
}

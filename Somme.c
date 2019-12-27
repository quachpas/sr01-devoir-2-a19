#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char const *argv[])
{
  if (argc < 3 ) {
    printf("Usage : ./Somme matrice1 matrice 2 n\n\n");
    return EXIT_FAILURE;
  }
  FILE* f;
  f=fopen(argv[1],"rb");
  int n=atof(argv[3]);//recuperation du n
  int** tableaux,i,*ligne,N;
  int *matrice,*matrice2,j;
  
  matrice=malloc(n*n*sizeof(int));
  matrice2=malloc(n*n*sizeof(int));
  //lecture des matrices
  if (f!=NULL){
    fread(matrice,sizeof(int),n*n,f);
  }else{
    printf("ouverture fichier 1 impossible\n");
    return 0;
  }
  fclose(f);


  f=fopen(argv[2],"rb");
  if (f!=NULL){
    fread(matrice2,sizeof(int),n*n,f);
  }else{
    printf("ouverture fichier 1 impossible\n");
    return 0;
  }
  fclose(f);
//affichage de l'opération
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
        printf("%3d ",matrice[i*n+j]);
    }
    if(i==n/2) printf(" + ");
    else printf("   ");
    for(j=0;j<n;j++){
        printf("%3d ",matrice2[i*n+j]);
    }
    printf("\n");
  }
//génération des pipes et des tableaux associés
  tableaux=malloc(n* sizeof(int*));
  for (i=0;i<n;i++){
    tableaux[i]=malloc(2*sizeof(int));
    pipe(tableaux[i]);

  }
//génération des processus
  for(i=0;i<n;i++){
    N = fork();
    if (N==0){//processus fils
        printf("Calcul de la ligne numero %d\n",i);
        ligne=malloc(n*sizeof(int));
        for(j=0;j<n;j++){//calcul d'une ligne du résultat
            ligne[j]=matrice[i*n+j]+matrice2[i*n+j];
        }
        write(tableaux[i][1],ligne,n*sizeof(int));//ecriture dans le pipe associé au processus
        free(ligne);
        exit(0);
    }

  }
  //attente des processus fils
  while(wait(NULL)>0);
  //affichage du résultat
  printf("Resultat:\n");
  ligne=malloc(n*sizeof(int));
  for(i=0;i<n;i++){
    read(tableaux[i][0],ligne,n*sizeof(int));//lecture du pipe
    for(j=0;j<n;j++){
        printf("%3d ",ligne[j]);//affichage
    }
    printf("\n");
  }
  free(ligne);
  free(matrice);
  free(matrice2);
  for (i=0;i<n;i++){
    free(tableaux[i]);

  }
  free(tableaux);
  return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


void * fctThread(); 
pthread_t threadHandle;

int main()
{
	int ret, * retThread; 

 	puts("Thread principal demarre"); 
 	ret = pthread_create(&threadHandle, NULL, (void *(*) (void *))fctThread, NULL);
 	puts("Thread secondaire lance !"); 
 	puts("Attente de la fin du thread secondaire"); 
	ret = pthread_join(threadHandle, (void **)&retThread); 
 	printf("Nombre d'occurence dans le fichier = %d\n", *retThread); 
	puts("Fin du thread principal"); 

	 return 0;
}



void * fctThread () 
{
	static int vr = 0; 
 
	int fd,nbrOctects;

	char mot[5],lecture[5];
	strcat(mot,"stru");

	 	fd = open("fichier",O_RDONLY);

	 	nbrOctects = lseek(fd,0,SEEK_END);
	 	printf("th1>Le Nombre de charactere du fichier : %d \n",nbrOctects);
	 	close(fd);

	 	puts("th1> Debut de thread"); 
	 	printf("th1> Valeur renvoyee par defaut = %d\n", vr); 
	 	for (int i = 0; i < nbrOctects-4; ++i)
	 	{
	 		fd=open("fichier",O_RDONLY);
	 		lseek(fd,i*sizeof(char),SEEK_CUR);
	 		read(fd,&lecture,4);
	 		if(strcmp(mot,lecture) ==0)
	 			{
	 				vr++;
				}
				close(fd);
	 	}


	 	printf("th1> Valeur renvoyee = %d\n", vr); 
		pthread_exit(&vr); 
	 return 0;
}
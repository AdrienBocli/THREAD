#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


void * fctThread1(); 
void * fctThread2();
void * fctThread3();
void * fctThread4();

pthread_t threadHandle1;
pthread_t threadHandle2;
pthread_t threadHandle3;
pthread_t threadHandle4;

int main()
{
	int ret, * retThread;
	int occ1,occ2,occ3,occ4;

 	ret = pthread_create(&threadHandle1, NULL, (void *(*) (void *))fctThread1, NULL);
 	puts("Thread 1 lance !"); 
 	ret = pthread_create(&threadHandle2, NULL, (void *(*) (void *))fctThread2, NULL);
 	puts("Thread 2 lance !"); 
 	ret = pthread_create(&threadHandle3, NULL, (void *(*) (void *))fctThread3, NULL);
 	puts("Thread 3 lance !"); 
 	ret = pthread_create(&threadHandle4, NULL, (void *(*) (void *))fctThread4, NULL);
 	puts("Thread 4 lance !"); 

	ret = pthread_join(threadHandle1, (void **)&retThread);
	occ1=*retThread;
	ret = pthread_join(threadHandle2, (void **)&retThread);
	occ2=*retThread;
	ret = pthread_join(threadHandle3, (void **)&retThread);
	occ3=*retThread;
	ret = pthread_join(threadHandle4, (void **)&retThread);
	occ4=*retThread;
 	printf("Nombre d'occurence dans le fichier1 = %d\n",occ1);
 	printf("Nombre d'occurence dans le fichier2 = %d\n",occ2);
 	printf("Nombre d'occurence dans le fichier3 = %d\n",occ3);
 	printf("Nombre d'occurence dans le fichier4 = %d\n",occ4);

 	/*ne pas oublier les free*/
	 return 0;
}



void * fctThread1 () 
{
	int* vr=(int *)malloc(sizeof(int));
	*vr=0; 
 
	int fd,nbrOctects,fin;
	fin=4;
	char mot[5],lecture[5];
	strcpy(mot,"Wiki");

	 	fd = open("fichier1",O_RDONLY);

	 	nbrOctects = lseek(fd,0,SEEK_END);
	 	printf("th1>Le Nombre de charactere du fichier : %d \n",nbrOctects);
	 	close(fd);

	 	puts("th1> Debut de thread"); 
	 	printf("th1> Valeur renvoyee par defaut = %d\n", *vr); 
	 	for (int i = 0;fin==4 ; ++i)
	 	{
	 		fd=open("fichier1",O_RDONLY);
	 		printf("	*\n");
	 		lseek(fd,i*sizeof(char),SEEK_CUR);
	 		fin=read(fd,&lecture,4);
	 		if(strcmp(mot,lecture) ==0)
	 			{
	 				(*vr)++;
				}
				close(fd);
	 	}


	 	printf("th1> Valeur renvoyee = %d\n", *vr); 
		pthread_exit(vr); 
	 return 0;
}

void * fctThread2 () 
{
	int* vr=(int *)malloc(sizeof(int));
	*vr=0; 
 
	int fd,nbrOctects,fin;
	fin=4;
	char mot[5],lecture[5];
	strcpy(mot,"guer");

	 	fd = open("fichier2",O_RDONLY);

	 	nbrOctects = lseek(fd,0,SEEK_END);
	 	printf("th1>Le Nombre de charactere du fichier : %d \n",nbrOctects);
	 	close(fd);

	 	puts("th1> Debut de thread"); 
	 	printf("th1> Valeur renvoyee par defaut = %d\n", *vr); 
	 	for (int i = 0;fin==4 ; ++i)
	 	{
	 		fd=open("fichier2",O_RDONLY);
	 		printf("		*\n");
	 		lseek(fd,i*sizeof(char),SEEK_CUR);
	 		fin=read(fd,&lecture,4);
	 		if(strcmp(mot,lecture) ==0)
	 			{
	 				(*vr)++;
				}
				close(fd);
	 	}


	 	printf("th2> Valeur renvoyee = %d\n", *vr); 
		pthread_exit(vr); 
	 return 0;
}

void * fctThread3 () 
{
	int* vr=(int *)malloc(sizeof(int));
	*vr=0; 
 
	int fd,nbrOctects,fin;
	fin=4;
	char mot[5],lecture[5];
	strcpy(mot,"Stal");

	 	fd = open("fichier3",O_RDONLY);

	 	nbrOctects = lseek(fd,0,SEEK_END);
	 	printf("th1>Le Nombre de charactere du fichier : %d \n",nbrOctects);
	 	close(fd);

	 	puts("th1> Debut de thread"); 
	 	printf("th1> Valeur renvoyee par defaut = %d\n", *vr); 
	 	for (int i = 0;fin==4 ; ++i)
	 	{
	 		fd=open("fichier3",O_RDONLY);
	 		printf("			*\n");
	 		lseek(fd,i*sizeof(char),SEEK_CUR);
	 		fin=read(fd,&lecture,4);
	 		if(strcmp(mot,lecture) ==0)
	 			{
	 				(*vr)++;
				}
				close(fd);
	 	}


	 	printf("th3> Valeur renvoyee = %d\n", *vr); 
		pthread_exit(vr); 
	 return 0;
}

void * fctThread4 () 
{
	int* vr=(int *)malloc(sizeof(int));
	*vr=0; 
 
	int fd,nbrOctects,fin;
	fin=4;
	char mot[5],lecture[5];
	strcpy(mot,"comm");

	 	fd = open("fichier4",O_RDONLY);

	 	nbrOctects = lseek(fd,0,SEEK_END);
	 	printf("th1>Le Nombre de charactere du fichier : %d \n",nbrOctects);
	 	close(fd);

	 	puts("th1> Debut de thread"); 
	 	printf("th1> Valeur renvoyee par defaut = %d\n", *vr); 
	 	for (int i = 0;fin==4 ; ++i)
	 	{
	 		fd=open("fichier4",O_RDONLY);
	 		printf("				*\n");
	 		lseek(fd,i*sizeof(char),SEEK_CUR);
	 		fin=read(fd,&lecture,4);
	 		if(strcmp(mot,lecture) ==0)
	 			{
	 				(*vr)++;
				}
				close(fd);
	 	}


	 	printf("th4> Valeur renvoyee = %d\n", *vr); 
		pthread_exit(vr); 
	 return 0;
}

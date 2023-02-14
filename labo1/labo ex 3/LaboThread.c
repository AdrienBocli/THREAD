#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


typedef struct 
{
	char NomFichier[10];
	char MotCherch[5];
	int NbrTab;
}LaboThread;

void * fctThread( LaboThread*); 

pthread_t threadHandle1;
pthread_t threadHandle2;
pthread_t threadHandle3;
pthread_t threadHandle4;

int main()
{
	int ret, * retThread1,* retThread2,* retThread3,* retThread4;
	int occ1,occ2,occ3,occ4;
	LaboThread  t1,t2,t3,t4;

	strcpy(t1.NomFichier,"fichier1");
	strcpy(t2.NomFichier,"fichier2");
	strcpy(t3.NomFichier,"fichier3");
	strcpy(t4.NomFichier,"fichier4");

	strcpy(t1.MotCherch,"Wiki");
	strcpy(t2.MotCherch,"Stal");
	strcpy(t3.MotCherch,"comm");
	strcpy(t4.MotCherch,"guer");

	t1.NbrTab=1;
	t2.NbrTab=2;
	t3.NbrTab=3;
	t4.NbrTab=4;


 	ret = pthread_create(&threadHandle1, NULL, (void *(*) (void *))fctThread, &t1);
 	puts("Thread 1 lance !"); 
 	ret = pthread_create(&threadHandle2, NULL, (void *(*) (void *))fctThread, &t2);
 	puts("Thread 2 lance !"); 
 	ret = pthread_create(&threadHandle3, NULL, (void *(*) (void *))fctThread, &t3);
 	puts("Thread 3 lance !"); 
 	ret = pthread_create(&threadHandle4, NULL, (void *(*) (void *))fctThread, &t4);
 	puts("Thread 4 lance !"); 

	ret = pthread_join(threadHandle1, (void **)&retThread1);
	occ1=*retThread1;
	ret = pthread_join(threadHandle2, (void **)&retThread2);
	occ2=*retThread2;
	ret = pthread_join(threadHandle3, (void **)&retThread3);
	occ3=*retThread3;
	ret = pthread_join(threadHandle4, (void **)&retThread4);
	occ4=*retThread4;
 	printf("Nombre d'occurence dans le fichier1 = %d\n",occ1);
 	printf("Nombre d'occurence dans le fichier2 = %d\n",occ2);
 	printf("Nombre d'occurence dans le fichier3 = %d\n",occ3);
 	printf("Nombre d'occurence dans le fichier4 = %d\n",occ4);

 	free(retThread1);
 	free(retThread2);
 	free(retThread3);
 	free(retThread4);

 	
	 return 0;
}



void * fctThread (LaboThread* thread) 
{
	int* vr=(int *)malloc(sizeof(int));
	*vr=0; 
 
	int fd,nbrOctects,fin;
	fin =strlen(thread->MotCherch);

	char lecture[10];

	 	fd = open(thread->NomFichier,O_RDONLY);

	 	nbrOctects = lseek(fd,0,SEEK_END);
	 	close(fd);

	 	for (int i = 0;fin==strlen(thread->MotCherch); ++i)
	 	{
	 		fd=open(thread->NomFichier,O_RDONLY);
	 		for(int j=0;j<thread->NbrTab;j++)
	 		{
	 			printf("	");
	 		}
	 		printf("*\n");
	 		lseek(fd,i*sizeof(char),SEEK_CUR);
	 		fin=read(fd,&lecture,fin);
	 		if(strcmp(thread->MotCherch,lecture) ==0)
	 			{
	 				(*vr)++;
				}
				close(fd);
	 	}


	 	printf("Valeur renvoyee = %d\n", *vr); 
		pthread_exit(vr);
	 return 0;
}

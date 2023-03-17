#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>


typedef struct
{ 
char nom[20];
int nbSecondes;
} DONNEE;


DONNEE data[] = { 	"MATAGNE",15,
					"WILVERS",10,
					"WAGNER",17,
					"QUETTIER",8,
					"",0 };
DONNEE Param;

void * fctThreadSlave(DONNEE *);

pthread_t threadHandle[sizeof(DONNEE)];
pthread_mutex_t mutexparam;

int compteur=0;
pthread_mutex_t mutexcomp;
pthread_cond_t condcomp;

int main()
{
	int ret, * retThread[sizeof(DONNEE)],i=0;

	pthread_mutex_init(&mutexparam,NULL);
	pthread_mutex_init(&mutexcomp,NULL);
	pthread_cond_init(&condcomp,NULL);
	

 	while(i<4)
 	{
 		pthread_mutex_lock(&mutexparam);
 		memcpy(&Param,&data[i],sizeof(DONNEE));
 		ret = pthread_create(&threadHandle[i], NULL, (void *(*) (void *))fctThreadSlave, &Param);
 		printf("Thread %d lance !\n",i); 
 		i++;
 		compteur++;
 		
 	}

 	pthread_mutex_lock(&mutexcomp);
 	while(compteur)
 	{
 		pthread_cond_wait(&condcomp,&mutexcomp);
 	}
 	
	 return 0;
}



void * fctThreadSlave(DONNEE *Param)
{

	struct timespec temps;

	temps.tv_sec= Param->nbSecondes;
	temps.tv_nsec=0;

	printf("th>Debut du Thread %u.%u\n",getpid(),pthread_self());
	printf("th>Le nom est : %s\n",Param->nom );
	pthread_mutex_unlock(&mutexparam);
	nanosleep(&temps,NULL);
	printf("th>Fin du Thread %u.%u\n",getpid(),pthread_self());

	pthread_mutex_lock(&mutexcomp);
	compteur--;
	pthread_mutex_unlock(&mutexcomp);
	pthread_cond_signal(&condcomp);
	


	return 0;
}
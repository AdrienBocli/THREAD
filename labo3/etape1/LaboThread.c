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



int main()
{
	int ret, * retThread[sizeof(DONNEE)],i=0;
	

 	while(i<4)
 	{
 		memcpy(&Param,&data[i],sizeof(DONNEE));
 		ret = pthread_create(&threadHandle[i], NULL, (void *(*) (void *))fctThreadSlave, &Param);
 		printf("Thread %d lance !",i); 
 		i++;
 	}
 	i=0;

 	while(i<4)
 	{
 		ret= pthread_join(threadHandle[i], (void **)&retThread[i]);
 		i++;
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
	nanosleep(&temps,NULL);
	printf("th>Fin du Thread %u.%u\n",getpid(),pthread_self());
	return 0;
}
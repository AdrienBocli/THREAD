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


DONNEE data[] = { 	"MATAGNE",5,
					"WILVERS",10,
					"WAGNER",15,
					"QUETTIER",20,
					"",0 };
DONNEE Param;

void * fctThreadSlave(DONNEE *);
void HandlerSIGINT(int sig);
void Destructeur(void*);

pthread_t threadHandle[sizeof(DONNEE)];
pthread_mutex_t mutexparam;

int compteur=0;
pthread_mutex_t mutexcomp;
pthread_cond_t condcomp;

pthread_key_t cle;

int main()
{
	int ret, * retThread[sizeof(DONNEE)],i=0;

	struct sigaction A;
	sigset_t mask;

	A.sa_handler = HandlerSIGINT;
	sigemptyset(&A.sa_mask);
	A.sa_flags =0;
	sigaction(SIGINT,&A,NULL);

	
	

 	pthread_key_create(&cle,Destructeur);


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
 	sigemptyset(&mask);
 	sigaddset(&mask,SIGINT);
 	sigprocmask(SIG_SETMASK,&mask,NULL);
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
	struct timespec tmpRest;

	char* n=(char*)malloc(sizeof(Param->nom));
	strcpy(n,Param->nom);
	pthread_setspecific(cle,n);

	temps.tv_sec= Param->nbSecondes;
	temps.tv_nsec=0;

	printf("th>Debut du Thread %u.%u\n",getpid(),pthread_self());
	printf("th>Le nom est : %s\n",Param->nom );
	pthread_mutex_unlock(&mutexparam);

	while(nanosleep(&temps, &tmpRest)== -1)
	{
		printf("Temps restant lors de l interruption de nanosleep :%d\n",tmpRest.tv_sec);
		temps.tv_sec = tmpRest.tv_sec;
		temps.tv_nsec = tmpRest.tv_nsec;
	}
	printf("th>Fin du Thread %u.%u\n",getpid(),pthread_self());

	pthread_mutex_lock(&mutexcomp);
	compteur--;
	pthread_mutex_unlock(&mutexcomp);
	pthread_cond_signal(&condcomp);


	pthread_exit(NULL);
	


	return 0;
}

void HandlerSIGINT(int sig)
{
	char*nom;
	nom=(char*)pthread_getspecific(cle);
	printf("\nJ'ai recu le signal SIGINT no %u.%u et s'occupe de %s\n",getpid(),pthread_self(),nom);

}

void Destructeur(void* n)
{
	printf("bien détruit");
	free(n);
}
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>



void * fctThreadSlave();
void * fctThreadMaster();
void HandlerSIGINT(int sig);
void HandlerSIGUSR1(int sig);

pthread_t threadHandle1;
pthread_t threadHandle2;
pthread_t threadHandle3;
pthread_t threadHandle4;
pthread_t threadHandle5;



int main()
{
	int ret, * retThread1,* retThread2,* retThread3,* retThread4,* retThread5;
	struct sigaction A;
	sigset_t mask;

	A.sa_handler = HandlerSIGINT;
	sigemptyset(&A.sa_mask);
	A.sa_flags= 0;

	sigaction(SIGINT,&A,NULL);

	A.sa_handler = HandlerSIGUSR1;

	sigaction(SIGUSR1,&A,NULL);


 	ret = pthread_create(&threadHandle1, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 1 lance !"); 
 	ret = pthread_create(&threadHandle2, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 2 lance !"); 
 	ret = pthread_create(&threadHandle3, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 3 lance !"); 
 	ret = pthread_create(&threadHandle4, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 4 lance !");
 	ret = pthread_create(&threadHandle5, NULL, (void *(*) (void *))fctThreadMaster, NULL);
 	puts("Thread 5 lance !"); 

 	sigemptyset(&mask);
 	sigaddset(&mask,SIGINT);
 	sigaddset(&mask,SIGUSR1);
 	sigprocmask(SIG_SETMASK,&mask,NULL);

 	ret= pthread_join(threadHandle1, (void **)&retThread1);
 	ret= pthread_join(threadHandle2, (void **)&retThread2);
 	ret= pthread_join(threadHandle3, (void **)&retThread3);
 	ret= pthread_join(threadHandle4, (void **)&retThread4);

 	
	 return 0;
}



void * fctThreadSlave () 
{

	sigset_t mask;

	sigemptyset(&mask);
 	sigaddset(&mask,SIGINT);
 	sigprocmask(SIG_SETMASK,&mask,NULL);

	puts("th> Debut de thread"); 
 	printf("th> identite = %u\n", pthread_self());
 	printf("th>Ce thread est va attendre un singal\n");
 	pause();

 	pthread_exit(NULL);
	 return 0;
}


void * fctThreadMaster ()
{
	sigset_t mask;

	sigemptyset(&mask);
 	sigaddset(&mask,SIGUSR1);
 	sigprocmask(SIG_SETMASK,&mask,NULL);

	puts("th> Debut de thread Master");
	printf("th> identite = %u\n", pthread_self());
	while(1)
	{
		pause();
	}
}

void HandlerSIGINT(int sig)
{
	printf("\nJ'ai recu le signal SIGINT no %u\n",pthread_self());

	kill(getpid(),SIGUSR1);

}

void HandlerSIGUSR1(int sig)
{
	printf("\nJ'ai recu le signal SIGUSR1 no %u\n",pthread_self());

	pthread_exit(NULL);
}
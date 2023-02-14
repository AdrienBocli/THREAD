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
void HandlerSIGINT(int sig);

pthread_t threadHandle1;
pthread_t threadHandle2;
pthread_t threadHandle3;
pthread_t threadHandle4;



int main()
{
	int ret, * retThread1,* retThread2,* retThread3,* retThread4;
	struct sigaction A;

	A.sa_handler = HandlerSIGINT;
	sigemptyset(&A.sa_mask);
	A.sa_flags= 0;

	sigaction(SIGINT,&A,NULL);

 	ret = pthread_create(&threadHandle1, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 1 lance !"); 
 	ret = pthread_create(&threadHandle2, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 2 lance !"); 
 	ret = pthread_create(&threadHandle3, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 3 lance !"); 
 	ret = pthread_create(&threadHandle4, NULL, (void *(*) (void *))fctThreadSlave, NULL);
 	puts("Thread 4 lance !"); 

 	pause();

 	pthread_exit(NULL);

 	
	 return 0;
}



void * fctThreadSlave () 
{

	puts("th> Debut de thread"); 
 	printf("th> identite = %u\n", pthread_self());
 	printf("th>Ce thread est va attendre un singal\n");
 	pause();

 	pthread_exit(NULL);
	 return 0;
}


void HandlerSIGINT(int sig)
{
	printf("\nJ'ai recu le signal no %u\n",pthread_self());

	pthread_exit(NULL);
}
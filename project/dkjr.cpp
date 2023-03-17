#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <SDL/SDL.h>
#include "./presentation/presentation.h"

#define VIDE        		0
#define DKJR       		1
#define CROCO       		2
#define CORBEAU     		3
#define CLE 			4

#define AUCUN_EVENEMENT    	0

#define LIBRE_BAS		1
#define LIANE_BAS		2
#define DOUBLE_LIANE_BAS	3
#define LIBRE_HAUT		4
#define LIANE_HAUT		5

void* FctThreadEvenements(void *);
void* FctThreadCle(void *);
void* FctThreadDK(void *);
void* FctThreadDKJr(void *);
void* FctThreadScore(void *);
void* FctThreadEnnemis(void *);
void* FctThreadCorbeau(void *);
void* FctThreadCroco(void *);

void initGrilleJeu();
void setGrilleJeu(int l, int c, int type = VIDE, pthread_t tid = 0);
void afficherGrilleJeu();

void HandlerSIGUSR1(int);
void HandlerSIGUSR2(int);
void HandlerSIGALRM(int);
void HandlerSIGINT(int);
void HandlerSIGQUIT(int);
void HandlerSIGCHLD(int);
void HandlerSIGHUP(int);

void DestructeurVS(void *p);

pthread_t threadCle;
pthread_t threadDK;
pthread_t threadDKJr;
pthread_t threadEvenements;
pthread_t threadScore;
pthread_t threadEnnemis;

pthread_cond_t condDK;
pthread_cond_t condScore;

pthread_mutex_t mutexGrilleJeu;
pthread_mutex_t mutexDK;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexScore;

pthread_key_t keySpec;

bool MAJDK = false;
int  score = 0;
bool MAJScore = true;
int  delaiEnnemis = 4000;
int  positionDKJr = 1;
int  evenement = AUCUN_EVENEMENT;
int etatDKJr;

typedef struct
{
  int type;
  pthread_t tid;
} S_CASE;

S_CASE grilleJeu[4][8];

typedef struct
{
  bool haut;
  int position;
} S_CROCO;

// ------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int evt;

	struct sigaction signaux;

	pthread_mutex_init(&mutexGrilleJeu,NULL);
	pthread_mutex_init(&mutexEvenement,NULL);


	ouvrirFenetreGraphique();

	initGrilleJeu();

	signaux.sa_handler = HandlerSIGQUIT;
	sigemptyset(&signaux.sa_mask);
	signaux.sa_flags = 0;
	sigaction(SIGQUIT,&signaux,NULL);

	pthread_create(&threadCle,NULL,(void*(*)(void*))FctThreadCle,NULL);
	pthread_create(&threadEvenements,NULL,(void*(*)(void*))FctThreadEvenements,NULL);
	pthread_create(&threadDKJr,NULL,(void*(*)(void*))FctThreadDKJr,NULL);

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGQUIT);
	sigprocmask(SIG_SETMASK,&mask,NULL);


	//pthread_join(threadCle,NULL);
	//pthread_join(threadEvenements,NULL);
	pthread_join(threadDKJr,NULL);
}

// -------------------------------------

void initGrilleJeu()
{
  int i, j;   
  
  pthread_mutex_lock(&mutexGrilleJeu);

  for(i = 0; i < 4; i++)
    for(j = 0; j < 7; j++)
      setGrilleJeu(i, j);

  pthread_mutex_unlock(&mutexGrilleJeu);
}

// -------------------------------------

void setGrilleJeu(int l, int c, int type, pthread_t tid)
{
  grilleJeu[l][c].type = type;
  grilleJeu[l][c].tid = tid;
}

// -------------------------------------

void afficherGrilleJeu()
{   
   for(int j = 0; j < 4; j++)
   {
       for(int k = 0; k < 8; k++)
          printf("%d  ", grilleJeu[j][k].type);
       printf("\n");
   }

   printf("\n");   
}

// -------------------------------------- FONCTION THREADS --------------------------------------------------------

void* FctThreadCle(void*p)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGQUIT);
	sigprocmask(SIG_SETMASK,&mask,NULL);

	printf("Lancement de la ThreadCle\n");
	struct timespec temps;
	int i = 1 ;
	int sens = 1;

	temps.tv_sec = 0;
	temps.tv_nsec = 700000000;

	while(1)
	{
		if(i == 1)
		{
			pthread_mutex_unlock(&mutexGrilleJeu);
			setGrilleJeu(0,1,CLE,threadCle);
			pthread_mutex_lock(&mutexGrilleJeu);
		}
		else
		{
			pthread_mutex_unlock(&mutexGrilleJeu);
			setGrilleJeu(0,1,VIDE,threadCle);
			pthread_mutex_lock(&mutexGrilleJeu);
		}
		afficherCle(i);


		if(sens)
		{
			i++;
			if(i==4)
			{
				sens = 0;
			}
		}

		else
		{
				i--;
				if(i==1)
				{
					sens = 1;
				}
		}

		nanosleep(&temps,NULL);
		effacerCarres(3,12,2,3);
		
	}
	pthread_exit(0);
}


void* FctThreadEvenements(void*p)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGQUIT);
	sigprocmask(SIG_SETMASK,&mask,NULL);

	int evt;
	struct timespec temps;

	temps.tv_sec = 0;
	temps.tv_nsec = 10000000;

	while(1)
	{
		evt = lireEvenement();
		pthread_mutex_lock(&mutexEvenement);
		switch(evt)
		{
		case SDL_QUIT:
			exit(0);
			break;
		case SDLK_UP:
			printf("KEY_UP\n");
			evenement = SDLK_UP;
			break;
		case SDLK_DOWN:
			printf("KEY_DOWN\n");
			evenement = SDLK_DOWN;
			break;
		case SDLK_LEFT:
			printf("KEY_LEFT\n");
			evenement = SDLK_LEFT;
			break;
		case SDLK_RIGHT:
			printf("KEY_RIGHT\n");
			evenement = SDLK_RIGHT;
			break;
	  }
	  pthread_mutex_unlock(&mutexEvenement);
	  kill(getpid(),SIGQUIT);
	  nanosleep(&temps,NULL);

	  pthread_mutex_lock(&mutexEvenement);
	  evenement = AUCUN_EVENEMENT;
	  pthread_mutex_unlock(&mutexEvenement);
	}
	}


void* FctThreadDKJr(void*p)
{
	bool on = true;
printf("wag1\n");
	pthread_mutex_lock(&mutexGrilleJeu);
printf("wag2\n");

	setGrilleJeu(3,1,DKJR);
	afficherDKJr(11,9,1);
	etatDKJr = LIBRE_BAS;
	positionDKJr = 1;

	pthread_mutex_unlock(&mutexGrilleJeu);

	while(on)
	{
		printf("passe dans la boucle while avnt le pause\n");
		pause();
		printf("passe dans la boucle while apres le pause\n");


		pthread_mutex_lock(&mutexEvenement);
		pthread_mutex_lock(&mutexGrilleJeu);
printf("passe dans la boucle while apres le unlock\n");

		switch(etatDKJr)
		{
			case LIBRE_BAS:
				printf("case bas\n");
				switch(evenement)
				{
					case SDLK_LEFT:
					if (positionDKJr > 1)
					{
						setGrilleJeu(3,positionDKJr);
						effacerCarres(11,(positionDKJr*2) +7,2,2);

						positionDKJr--;

						setGrilleJeu(3,positionDKJr,DKJR);
						afficherDKJr(11,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);
					}

					printf("THreadDKJr touche vers la gauche\n");
					break;

					case SDLK_RIGHT:
					if (positionDKJr < 7)
					{
						setGrilleJeu(3,positionDKJr);
						effacerCarres(11,(positionDKJr*2)+7,2,2);

						positionDKJr++;

						setGrilleJeu(3,positionDKJr,DKJR);
						afficherDKJr(11,(positionDKJr*2)+7,((positionDKJr - 1)%4)+1);
					}
					printf("THreadDKJr touche vers la droit\n");
					break;

					default:printf("XXXXXXXXXXXXXXXXX\n");
				}
			}
	 pthread_mutex_unlock(&mutexGrilleJeu);
	 pthread_mutex_unlock(&mutexEvenement);
	}
	pthread_exit(0);
}


// -------------------------------------- FONCTION HANDLER --------------------------------------------------------


void HandlerSIGQUIT(int signal)
{
	printf("Réception du signal SIGQUIT\n");
}
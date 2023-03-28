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
	int evt,vieperdue = 0;

	struct sigaction signaux;

	pthread_mutex_init(&mutexGrilleJeu,NULL);
	pthread_mutex_init(&mutexEvenement,NULL);
	pthread_mutex_init(&mutexDK,NULL);


	ouvrirFenetreGraphique();

	initGrilleJeu();


	signaux.sa_handler = HandlerSIGQUIT;
	sigemptyset(&signaux.sa_mask);
	signaux.sa_flags = 0;
	sigaction(SIGQUIT,&signaux,NULL);

	pthread_create(&threadCle,NULL,(void*(*)(void*))FctThreadCle,NULL);
	pthread_create(&threadEvenements,NULL,(void*(*)(void*))FctThreadEvenements,NULL);
	pthread_create(&threadDK,NULL,(void*(*)(void*))FctThreadDK,NULL);

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGQUIT);
	sigprocmask(SIG_SETMASK,&mask,NULL);

	while(vieperdue < 3)
	{
		pthread_create(&threadDKJr,NULL,FctThreadDKJr,NULL);
		pthread_join(threadDKJr,NULL);
		vieperdue ++;
		afficherEchec(vieperdue);
	}

	
	pthread_join(threadEvenements,NULL);
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
			pthread_mutex_lock(&mutexGrilleJeu);
			setGrilleJeu(0,1,CLE,threadCle);
			pthread_mutex_unlock(&mutexGrilleJeu);
		}
		else
		{
			pthread_mutex_lock(&mutexGrilleJeu);
			setGrilleJeu(0,1,VIDE,threadCle);
			pthread_mutex_unlock(&mutexGrilleJeu);
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

	sigset_t mask;
	sigemptyset(&mask);
	sigprocmask(SIG_SETMASK,&mask,NULL);

	bool on = true;
	pthread_mutex_lock(&mutexGrilleJeu);
	struct timespec temps;
	struct timespec temps2;

	temps.tv_sec = 1;
	temps.tv_nsec = 400000000;

	temps2.tv_sec = 0;
	temps2.tv_nsec = 500000000;



	setGrilleJeu(3,1,DKJR);
	afficherDKJr(11,9,1);
	effacerCarres(11,7,2,2);
	etatDKJr = LIBRE_BAS;
	positionDKJr = 1;


	pthread_mutex_unlock(&mutexGrilleJeu);

	while(on)
	{
		pause();

		pthread_mutex_lock(&mutexEvenement);
		pthread_mutex_lock(&mutexGrilleJeu);

		switch(etatDKJr)
		{
			case LIBRE_BAS:
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


					case SDLK_UP:

					setGrilleJeu(3,positionDKJr);
					effacerCarres(11,(positionDKJr*2)+7,2,2);

					setGrilleJeu(2,positionDKJr,DKJR);
					if(positionDKJr == 7)
					{
						etatDKJr = DOUBLE_LIANE_BAS;
						afficherDKJr(10,(positionDKJr*2)+7,5);
						printf("DOUBLE_LIANE_BAS\n");
					}
					else
					{
						if (positionDKJr == 1 || positionDKJr == 5)
						{
							etatDKJr = LIANE_BAS;
							afficherDKJr(10,(positionDKJr*2)+7,7);
							printf("LIANE_BAS\n");
						}
						else
						{
							afficherDKJr(10,(positionDKJr*2)+7,8);

						printf("THreadDKJr saut sans liane \n");

						pthread_mutex_unlock(&mutexGrilleJeu);
						nanosleep(&temps,NULL);
						pthread_mutex_lock(&mutexGrilleJeu);

						setGrilleJeu(2,positionDKJr);
						effacerCarres(10,(positionDKJr*2)+7,2,2);

						setGrilleJeu(3,positionDKJr,DKJR);
						afficherDKJr(11,(positionDKJr*2)+7,((positionDKJr -1)%4)+1);

						printf("THreadDKJr Effet de gravité retombe\n");
						}
					}
					break;
				}
				break;




			case LIANE_BAS:
			switch(evenement)
			{
				case SDLK_DOWN:
				
						etatDKJr=LIBRE_BAS;

						setGrilleJeu(2,positionDKJr);
						effacerCarres(10,(positionDKJr*2)+7,2,2);

						setGrilleJeu(3,positionDKJr,DKJR);
						afficherDKJr(11,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);

						printf("Descendre de LIANE_BAS");
						break;
			}
			break;


			case DOUBLE_LIANE_BAS:
			switch(evenement)
			{
				case SDLK_DOWN:
				etatDKJr=LIBRE_BAS;

				setGrilleJeu(2,positionDKJr);
				effacerCarres(10,(positionDKJr*2)+7,2,2);

				setGrilleJeu(3,positionDKJr,DKJR);
				afficherDKJr(11,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);

				printf("Descendre de DOUBLE_LIANE_BAS");
				break;

				case SDLK_UP:
				etatDKJr=LIBRE_HAUT;

				setGrilleJeu(2,positionDKJr);
				effacerCarres(10,(positionDKJr*2)+7,2,2);

				setGrilleJeu(1,positionDKJr,DKJR);
				afficherDKJr(7,(positionDKJr*2)+7,6);

				printf("Monte de DOUBLE_LIANE_BAS");
			}
			break;

			case LIBRE_HAUT:
			switch(evenement)
			{
				case SDLK_DOWN:
				if (positionDKJr == 7)
				{
					etatDKJr = DOUBLE_LIANE_BAS;

					setGrilleJeu(1,positionDKJr);
					effacerCarres(7,(positionDKJr*2)+7,2,2);

					setGrilleJeu(2,positionDKJr,DKJR);
					afficherDKJr(10,(positionDKJr*2)+7,5);

					printf("Decendre de LIBRE_HAUT\n");
				}
				break;

				case SDLK_LEFT:
				if (positionDKJr > 3)
				{
					setGrilleJeu(1,positionDKJr);
					effacerCarres(7,(positionDKJr*2)+7,2,2);

					positionDKJr--;

					setGrilleJeu(1,positionDKJr,DKJR);
					afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);

					printf("Touche gauche LIBRE_HAUT\n");
				}
				else
				{
					

					if(grilleJeu[0][1].type == CLE)
					{
						setGrilleJeu(1,positionDKJr);
						effacerCarres(7,(positionDKJr*2)+7,2,2);

						etatDKJr = LIBRE_BAS;

						afficherDKJr(5,12,9);
						nanosleep(&temps2,NULL);
						effacerCarres(5,12,3,2);

						effacerCarres(3,12,2,4);

						afficherDKJr(3,11,10);

						nanosleep(&temps2,NULL);
						effacerCarres(3,11,3,2);
						afficherCage(4);

						MAJDK = true;

						pthread_cond_signal(&condDK);

						positionDKJr = 1;

						setGrilleJeu(3,positionDKJr,DKJR);
						afficherDKJr(11,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);
					}
					else
					{
					setGrilleJeu(1,positionDKJr);
					effacerCarres(7,(positionDKJr*2)+7,2,2);

					etatDKJr = LIBRE_BAS;

					afficherDKJr(14,9,9);
					pthread_mutex_unlock(&mutexGrilleJeu);
					nanosleep(&temps2,NULL);
					pthread_mutex_lock(&mutexGrilleJeu);
					effacerCarres(5,12,3,2);
					

					afficherDKJr(6,11,12);
					pthread_mutex_unlock(&mutexGrilleJeu);
					nanosleep(&temps2,NULL);
					pthread_mutex_lock(&mutexGrilleJeu);
					effacerCarres(6,11,2,2);

					afficherDKJr(14,9,13);
					pthread_mutex_unlock(&mutexGrilleJeu);
					nanosleep(&temps2,NULL);
					pthread_mutex_lock(&mutexGrilleJeu);

					positionDKJr = 0;
					setGrilleJeu(3,positionDKJr,DKJR);

					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_mutex_unlock(&mutexEvenement);
					pthread_exit(0);
					}
				}
				break;
				case SDLK_RIGHT:
				if (positionDKJr<6)
				{
					setGrilleJeu(1,positionDKJr);
					effacerCarres(7,(positionDKJr*2)+7,2,2);

					positionDKJr++;

					setGrilleJeu(1,positionDKJr,DKJR);
					afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);

					printf("Vers la droite\n");
				}
				else
				{
					if (positionDKJr == 6)
					{
						setGrilleJeu(1,positionDKJr);
						effacerCarres(7,(positionDKJr*2)+7,2,2);

						positionDKJr++;

						setGrilleJeu(1,positionDKJr,DKJR);
						afficherDKJr(7,(positionDKJr*2)+7,6);
						printf("Vers la droite\n");
					}
				}
				
				break;

				case SDLK_UP:
				if (positionDKJr == 3||positionDKJr == 4)
				{
					setGrilleJeu(1,positionDKJr);
					effacerCarres(7,(positionDKJr*2)+7,2,2);

					setGrilleJeu(0,positionDKJr,DKJR);
					afficherDKJr(6,(positionDKJr*2)+7,8);

					printf("Saut pthreadDKjr LIANE_HAUT\n");

					pthread_mutex_unlock(&mutexGrilleJeu);
					nanosleep(&temps,NULL);
					pthread_mutex_lock(&mutexGrilleJeu);

					setGrilleJeu(0,positionDKJr);
					effacerCarres(6,(positionDKJr*2)+7,2,2);

					setGrilleJeu(1,positionDKJr,DKJR);
					afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);

					printf("Gravite \n");
				}
				else
				{
					if (positionDKJr == 6)
					{
						etatDKJr = LIANE_HAUT;

						setGrilleJeu(1,positionDKJr);
						effacerCarres(7,(positionDKJr*2)+7,2,2);

						setGrilleJeu(0,positionDKJr,DKJR);
						afficherDKJr(6,(positionDKJr*2)+7,7);

						printf("LIANE_HAUT\n");
					}
				}
				break;
			}
			break;

			case LIANE_HAUT:
			{
				switch(evenement)
				{
					case SDLK_DOWN:
					etatDKJr = LIBRE_HAUT;

					setGrilleJeu(0,positionDKJr);
					effacerCarres(6,(positionDKJr*2)+7,2,2);

					setGrilleJeu(1,positionDKJr,DKJR);
					afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);
					
					printf("Descendre LIANE_HAUT\n");
				}
			}
		}
	 pthread_mutex_unlock(&mutexGrilleJeu);
	 pthread_mutex_unlock(&mutexEvenement);
	}
	pthread_exit(0);
}

void* FctThreadDK(void*p)
{

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGQUIT);
	sigprocmask(SIG_SETMASK,&mask,NULL);


	afficherCage(1);
	afficherCage(2);
	afficherCage(3);
	afficherCage(4);

	struct timespec temps;

	temps.tv_sec = 0;
	temps.tv_nsec = 700000000;

	int i = 1;

	while(1)
	{
		pthread_cond_wait(&condDK,&mutexDK);

		if (MAJDK == true)
		{
			switch(i)
			{
				case 1:
					effacerCarres(2,7,2,2);
					i++;
					break;
				case 2:
					effacerCarres(2,9,2,2);
					i++;
					break;
				case 3:
					effacerCarres(4,7,2,2);
					i++;
					break;
				case 4:
					effacerCarres(4,9,2,3);
					afficherRireDK();
					nanosleep(&temps,NULL);
					effacerCarres(3,8,2,2);
					i=1;

					afficherCage(1);
					afficherCage(2);
					afficherCage(3);
					afficherCage(4);

			}
		}
		MAJDK == false;
	}
}


// -------------------------------------- FONCTION HANDLER --------------------------------------------------------


void HandlerSIGQUIT(int signal)
{
	printf("Réception du signal SIGQUIT\n");
}